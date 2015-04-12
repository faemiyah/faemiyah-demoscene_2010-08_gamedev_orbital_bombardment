#include "ob_game_view.hpp"

#include "math/random.hpp"
#include "gfx/mesh_static.hpp"
#include "ob_constants.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

/** How fast the escape speed increments. */
static const float ESCAPE_ACCELERATION = 0.009f;

/** Player death time. */
static const int DEATH_TIME = 350;

/** Death explosion color. */
static const gfx::Color PARTICLE_DEATH_EXPLOSION_COLOR(1.0f, 0.8f, 0.4f, 0.6f);

/** Death smoke color. */
static const gfx::Color PARTICLE_DEATH_SMOKE_COLOR(0.4f, 0.4f, 0.45f, 0.7f);

/** Death spark color. */
static const gfx::Color PARTICLE_DEATH_SPARK_COLOR(1.0f, 0.4f, 0.1f, 0.8f);

/** Death Explosion size. */
static const float DEATH_PARTICLE_SIZE = 2.0f;

/** Death particle lifetime. */
static const int DEATH_PARTICLE_TIME = 80;

/** Player collision box. */
static const float OB_COLLISION_PLAYER = 1.0f;

/** Camera distance from siege cruiser. */
static const float OB_CAMERA_HOVER = 9.0f;

/** Camera offset from siege cruiser. */
static const float OB_CAMERA_OFFSET = 3.0f;

/** Camera offset angle modification speed. */
static const float OB_CAMERA_OFFSET_SPEED = static_cast<float>(M_PI) / 85.0f;

/** Fade in rotation level limit. */
static const float FADE_LIMIT = OB_PLAY_ORBIT_LIMIT + 1000.0f;

GameView::GameView(float orbit, float rlimit) :
  EntityObject(math::vec3d(math::mrand(-1.0, 1.0), math::mrand(-1.0, 1.0), math::mrand(-1.0, 1.0)),
      math::vec3d(0.0, 0.0, 0.0), gfx::Mesh::locate("siegecruiser")),
  CollisionElement(OB_COLLISION_PLAYER, POINT, OB_FACTION_PLAYER),
  m_dir(math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f)),
  m_escape_speed(0.0f),
  m_offset_angle(0.0f),
  m_offset_angle_target(0.0f),
  m_offset_angle_speed(0.0f),
  m_orbit(orbit),
  m_rlimit(rlimit),
  m_time_to_course_change(-1),
  m_time_to_course_normalization(-1),
  m_flag_course_change(false)
{
  this->initCollisionData(math::vec3f(m_pos));
}

GameView::GameView(float orbit, float rlimit, const math::vec3d &dir, const math::vec3d &pos,
    const math::vec3d &rot) :
  EntityObject(pos, rot, gfx::Mesh::locate("siegecruiser")),
  CollisionElement(OB_COLLISION_PLAYER, POINT, OB_FACTION_PLAYER),
  m_dir(dir),
  m_escape_speed(0.0f),
  m_offset_angle(0.0f),
  m_offset_angle_target(0.0f),
  m_offset_angle_speed(0.0f),
  m_orbit(orbit),
  m_rlimit(rlimit),
  m_time_to_course_change(-1),
  m_time_to_course_normalization(-1),
  m_flag_course_change(false)
{
  this->initCollisionData(math::vec3f(m_pos));
}

void GameView::acceptCourse()
{
  if(m_flag_course_change)
  {
    snd::play(glob->getSampleRouteChangeAccepted());
    m_time_to_course_change = OB_COURSE_CHANGE_TIME;
    m_flag_course_change = false;
  }
}

void GameView::cancelCourse()
{
  m_flag_course_change = false;
}

void GameView::gamisticEffect(void *args)
{
  boost::ignore_unused_variable_warning(args);

  if(!this->isDead())
  {
    this->removeFromAreas();
    this->die(DEATH_TIME);
  }
}

void GameView::incEscapeSpeed()
{
  m_escape_speed += ESCAPE_ACCELERATION;
}

void GameView::spawnExplosion()
{
  math::vec3f floatdir(m_dir * OB_ORBIT_SPEED);
  math::vec3f floatpos(m_pos);

  // First, spawn increasing crackling for 1.5 seconds
  if(m_age > DEATH_TIME - 150)
  {
    for(int ii=0; ii<(DEATH_TIME - (m_age - 150))/100; ii++)
    {
      game->addParticle(Particle::randomCrackleParticle(),
          Particle(PARTICLE_DEATH_SPARK_COLOR,
            floatpos + math::vec3f(math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f)),
            DEATH_PARTICLE_SIZE,
            math::mrand(0.8f, 1.0f) * floatdir + math::vec3f(math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f)),
            DEATH_PARTICLE_TIME * 2, -DEATH_PARTICLE_SIZE));
      game->addParticle(Particle::randomSmokeHardParticle(),
          Particle(PARTICLE_DEATH_SMOKE_COLOR,
            floatpos + math::vec3f(math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f)),
            DEATH_PARTICLE_SIZE,
            math::mrand(0.8f, 1.0f) * floatdir + math::vec3f(math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f), math::mrand(-1.0f, 1.0f)),
            DEATH_PARTICLE_TIME * 2, -DEATH_PARTICLE_SIZE));
    }
  }
  else if(m_age > DEATH_TIME-300)
  {
    //Explode the fuck out
    // 50 large explosions per second
    if(math::mrand(0,3)<1)
    {
      game->addParticle(Particle::randomShockwaveWideParticle(),
          Particle(PARTICLE_DEATH_EXPLOSION_COLOR,
            floatpos + math::vec3f(math::mrand(-1.5f, 1.5f), math::mrand(-1.5f, 1.5f), math::mrand(-1.5f, 1.5f)),
            0,
            floatdir + math::vec3f(math::mrand(-0.5f, 0.5f), math::mrand(-0.5f, 0.5f), math::mrand(-0.5f, 0.5f)),
            static_cast<int>(DEATH_PARTICLE_TIME * 0.75f), 2.5f*DEATH_PARTICLE_SIZE));
    }
  }

  // One-time effect
  if(m_age == DEATH_TIME-150)
  {
    //BIG explosion after crackling
    for(int ii=0; ii<200; ii++)
    {
      game->addParticle(Particle::randomSmokeSoftParticle(),
          Particle(PARTICLE_DEATH_SPARK_COLOR,
            floatpos + math::vec3f(math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f)),
            0.5f*DEATH_PARTICLE_SIZE,
            math::mrand(0.97f, 1.0f) * floatdir + math::vec3f(math::mrand(-3.0f, 3.0f), math::mrand(-3.0f, 3.0f), math::mrand(-3.0f, 3.0f)),
            DEATH_PARTICLE_TIME * 5, 0.2f*DEATH_PARTICLE_SIZE));
      game->addParticle(Particle::randomSmokeHardParticle(),
          Particle(PARTICLE_DEATH_SMOKE_COLOR,
            floatpos + math::vec3f(math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f), math::mrand(-0.75f, 0.75f)),
            0.5f*DEATH_PARTICLE_SIZE,
            math::mrand(0.97f, 1.0f) * floatdir + math::vec3f(math::mrand(-3.0f, 3.0f), math::mrand(-3.0f, 3.0f), math::mrand(-3.0f, 3.0f)),
            DEATH_PARTICLE_TIME * 5, 0.5f*DEATH_PARTICLE_SIZE));
    }
    this->disableDraw();
  }
}

void GameView::rotate(double rx, double ry)
{
  m_rot.x() = math::min(math::max(rx + m_rot.x(), static_cast<double>(-1.0)),
      static_cast<double>(1.0));
  m_rot.y() = math::min(math::max(ry + m_rot.y(), static_cast<double>(-1.0)),
      static_cast<double>(1.0));
}

void GameView::updateCourse(const math::vec3d &tgt)
{
  // If accepted earlier course, don't do anything.
  if((0 <= m_time_to_course_change) || (0 <= m_time_to_course_normalization))
  {
    return;
  }
  if(!m_flag_course_change)
  {
    snd::play(glob->getSampleRouteChange());
    m_flag_course_change = true;
  }

  double course_time = static_cast<double>(OB_COURSE_CHANGE_TIME +
      OB_COURSE_NORMALIZATION_TIME);
  double lsphere = course_time * OB_ORBIT_SPEED * game->getTimestep(),
         lr = lsphere / (OB_PLAY_ORBIT * 2.0);
  math::vec3d norm_pos = math::normalize(m_pos),
    course_norm = math::cos(lr) * norm_pos + math::sin(lr) * m_dir,
    course_pos = course_norm * OB_PLAY_ORBIT;

  m_course_dir = math::normalize(tgt - course_pos);

  // Save to course matrix.
  m_course_matrix.loadLookAt(math::vec3f(course_pos),
      math::vec3f(0.0f, 0.0f, 0.0f),
      math::vec3f(m_course_dir));
  m_course_matrix(0, 3) = static_cast<float>(course_pos.x() * 0.01);
  m_course_matrix(1, 3) = static_cast<float>(course_pos.y() * 0.01);
  m_course_matrix(2, 3) = static_cast<float>(course_pos.z() * 0.01);
}

bool GameView::update()
{
  // Convergence doesn't take account of the orbit direction per se.
  m_orbit += m_escape_speed;
  if(m_orbit > OB_PLAY_ORBIT)
  {
    m_orbit -= (math::sqrt(m_orbit / OB_PLAY_ORBIT) - 1.0f) * OB_ORBIT_CONVERGE;
  }
  else
  {
    m_orbit += (1.0f - math::sqrt(m_orbit / OB_PLAY_ORBIT)) * OB_ORBIT_CONVERGE;
  }

  // Course change logic.
  if(0 <= m_time_to_course_change)
  {
    if(0 > --m_time_to_course_change)
    {
      m_time_to_course_normalization = OB_COURSE_NORMALIZATION_TIME;
      m_old_dir = m_dir;
    }
    m_course_visible.updateVisibility(false);
    m_course_change_visible.updateVisibility(true);
  }
  // Course updates.
  if(0 <= m_time_to_course_normalization)
  {
    double ttl = static_cast<double>(m_time_to_course_normalization) /
      static_cast<double>(OB_COURSE_NORMALIZATION_TIME);
    double ittl = 1.0 - ttl;
    m_pos += (m_course_dir * ittl + m_old_dir * ttl) *
      OB_ORBIT_SPEED * game->getTimestep();

    // Slower & sharper turn for camera.
    ttl = ttl * math::sqrt(ttl);
    ittl = ittl * math::sqrt(ittl);
    double sum = ttl + ittl;
    ttl = ttl / sum;;
    ittl = ittl / sum;;
    m_dir = (m_course_dir * ittl + m_old_dir * ttl) *
      OB_ORBIT_SPEED * game->getTimestep();

    if(0 > --m_time_to_course_normalization)
    {
      m_dir = m_course_dir;
    }
    m_course_visible.updateVisibility(false);
    m_course_change_visible.updateVisibility(false);
  }
  else
  {
    m_pos += m_dir * OB_ORBIT_SPEED * game->getTimestep();

    if(0 > m_time_to_course_change)
    {
      //std::cout << m_flag_course_change << std::endl;
      m_course_visible.updateVisibility(m_flag_course_change);
      m_course_change_visible.updateVisibility(m_flag_course_change);
    }
  }

  float adiff = math::abs(m_offset_angle - m_offset_angle_target);
  if(adiff > M_PI)
  {
    adiff = static_cast<float>(M_PI) - (adiff - static_cast<float>(M_PI));
  }
  float inc = adiff / static_cast<float>(M_PI) * m_offset_angle_speed,
        tgtdiff = m_offset_angle_target - m_offset_angle;
  if((tgtdiff > M_PI) || ((tgtdiff < 0.0f) && (tgtdiff > -M_PI)))
  {
    m_offset_angle -= inc;
  }
  else
  {
    m_offset_angle += inc;
  }
  //std::cout << m_offset_angle << std::endl;
  
  if(this->isDead())
  {
    this->spawnExplosion();
    return this->decrementAge();
  }
  else if(0.0f == m_escape_speed)
  {
    // Collision stuff is only updated if not escaping (i.e. the game is not
    // won yet.
    this->updateCollisionData(math::vec3f(m_pos));
    this->updateAreas(game->getOctree());
    CollisionElement *other = this->checkCollisions();
    if(other)
    {
      //std::cout << "collision!\n";
      other->gamisticEffect(NULL);
      this->gamisticEffect(NULL);
      return true;
    }
  }
  else
  {
    this->removeFromAreas();
  }
  return true;
}

const math::mat4f& GameView::viewOrbit()
{
  double rx = m_rot.x() * m_rlimit,
         ry = m_rot.y() * m_rlimit;

  this->bringToOrbit();

  math::mat4f matx, maty;
  matx.loadRotX(static_cast<float>(rx));
  maty.loadRotY(static_cast<float>(ry));
  math::mat4f vw = ((matx * maty) + (maty * matx)) * 0.5f;

  m_orbit_matrix.loadLookAt(math::vec3f(m_pos),
      math::vec3f(0.0f, 0.0f, 0.0f),
      math::vec3f(m_dir));
  m_dir = math::vec3d(static_cast<double>(m_orbit_matrix(0, 1)),
      static_cast<double>(m_orbit_matrix(1, 1)),
      static_cast<double>(m_orbit_matrix(2, 1)));
  m_orbit_matrix(0, 3) = 0.0f;
  m_orbit_matrix(1, 3) = 0.0f;
  m_orbit_matrix(2, 3) = 0.0f;

  // Create player matrix after all this hassle.
  m_wm.loadLookAt(math::vec3f(m_pos),
      math::vec3f(m_pos + m_dir),
      math::vec3f(m_pos));

  // Normal operation.
  math::vec2f ovec(math::cos(m_offset_angle), math::sin(m_offset_angle)),
    rvec(static_cast<float>(ry), -static_cast<float>(rx));
  float tangle = math::angle(rvec);
  m_offset_angle_target = tangle;
  m_offset_angle_speed = math::length2(rvec) / (m_rlimit * m_rlimit * 2.0f) *
    OB_CAMERA_OFFSET_SPEED;

  float offset = OB_CAMERA_OFFSET * (1.0f + m_escape_speed),
        hover = OB_CAMERA_HOVER * (1.0f + m_escape_speed);
  math::vec3f camera_offset =
    math::vec3f(ovec.x() * offset, ovec.y() * offset, hover);
  math::vec4f ladd = m_orbit_matrix * vw *
    math::vec4f(camera_offset.x(), camera_offset.y(), camera_offset.z(), 1.0f);
  math::vec3f pos_add(ladd.x(), ladd.y(), ladd.z());

  // Start fade-in.
  math::vec3f fw(-m_orbit_matrix(0, 2),
      -m_orbit_matrix(1, 2),
      -m_orbit_matrix(2, 2));
  if(FADE_LIMIT < m_orbit)
  {
    math::vec3f dn(-m_orbit_matrix(0, 1),
        -m_orbit_matrix(1, 1),
        -m_orbit_matrix(2, 1));
    float dist = FADE_LIMIT / m_orbit;
    pos_add += (1.0f - dist) * (dn * (OB_PLANET_RADIUS * 1.2f) - m_pos + fw * OB_PLANET_RADIUS) +
      (fw * OB_CAMERA_HOVER);
  }
  else if(OB_PLAY_ORBIT_LIMIT < m_orbit)
  {
    pos_add += (m_orbit - OB_PLAY_ORBIT_LIMIT) / (FADE_LIMIT - OB_PLAY_ORBIT_LIMIT) *
      (fw * OB_CAMERA_HOVER);
  }
  math::mat4f la;
  la.loadLookAt(math::vec3f(m_pos) + pos_add,
      math::vec3f(0.0f, 0.0f, 0.0f),
      math::vec3f(m_dir));

  m_camera_matrix = la * vw;
  m_camera_matrix.convertToView();
  return m_camera_matrix;
}

