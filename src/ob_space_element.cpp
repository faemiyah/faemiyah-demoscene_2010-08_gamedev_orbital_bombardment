#include "ob_space_element.hpp"

#include "ob_game.hpp"
#include "ob_globals.hpp"

using namespace ob;

math::vec3d SpaceElement::calculateMissileAimDirection(math::vec3d target_pos,
    math::vec3d target_vel, math::vec3d missile_pos, math::vec3d missile_vel)
{
  return math::normalize(calculateMissileAimTarget(target_pos, target_vel, missile_pos, missile_vel) - missile_pos);
}

math::vec3d SpaceElement::calculateMissileAimTarget(math::vec3d target_pos,
    math::vec3d target_vel, math::vec3d missile_pos, math::vec3d missile_vel)
{
  double time_to_target = math::length(target_pos - missile_pos) / math::length(missile_vel - target_vel);
  math::vec3d hit_location = target_pos + time_to_target * target_vel;
  return hit_location;
}

math::vec3d SpaceElement::calculateBulletAimTarget(math::vec3d target_pos,
    math::vec3d target_vel, math::vec3d launcher_pos, math::vec3d launcher_vel, double bullet_vel)
{
  math::vec3d p_c = target_pos - launcher_pos;
  math::vec3d v_c = target_vel - launcher_vel;

  double a = bullet_vel*bullet_vel - v_c.x()*v_c.x() - v_c.y()*v_c.y() - v_c.z()*v_c.z();
  double b = - 2*v_c.x()*p_c.x() - 2*v_c.y()*p_c.y() - 2*v_c.x()*p_c.x();
  double c = - p_c.x()*p_c.x() - p_c.y()*p_c.y() - p_c.z()*p_c.z();

  double determ = b*b-4*a*c;
  double t, tm, tx;
  if (determ < 0) return math::vec3d(0.0, 0.0, 0.0);
  else
  {
    double t1 = (-b + math::sqrt(determ)) / (2*a);
    double t2 = (-b - math::sqrt(determ)) / (2*a);
    tm = math::min(t1,t2);
    tx = math::max(t1,t2);
    if(tm>0) t=tm;
    else if(tx>0) t=tx;
    else return math::vec3d(0.0, 0.0, 0.0);
  }

  return math::vec3d(target_pos + t*target_vel);
}

math::vec3d SpaceElement::calculateBulletAimDirection(math::vec3d target_pos,
    math::vec3d target_vel, math::vec3d launcher_pos, math::vec3d launcher_vel, double bullet_vel)
{
  math::vec3d p_c = target_pos - launcher_pos;
  math::vec3d v_c = target_vel - launcher_vel;

  double a = bullet_vel*bullet_vel - v_c.x()*v_c.x() - v_c.y()*v_c.y() - v_c.z()*v_c.z();
  double b = - 2*v_c.x()*p_c.x() - 2*v_c.y()*p_c.y() - 2*v_c.x()*p_c.x();
  double c = - p_c.x()*p_c.x() - p_c.y()*p_c.y() - p_c.z()*p_c.z();

  double determ = b*b-4*a*c;
  double t, tm, tx;
  if (determ < 0) return math::vec3d(0.0, 0.0, 0.0);
  else
  {
    double t1 = (-b + math::sqrt(determ)) / (2*a);
    double t2 = (-b - math::sqrt(determ)) / (2*a);
    tm = math::min(t1,t2);
    tx = math::max(t1,t2);
    if(tm>0) t=tm;
    else if(tx>0) t=tx;
    else return math::vec3d(0.0, 0.0, 0.0);
  }

  double v_bx = (p_c.x() + v_c.x()*t)/(bullet_vel*t);
  double v_by = (p_c.y() + v_c.y()*t)/(bullet_vel*t);
  double v_bz = (p_c.z() + v_c.z()*t)/(bullet_vel*t);

  return math::vec3d(v_bx, v_by, v_bz);
}

double SpaceElement::calculateBulletTimeOfFlight(math::vec3d target_pos,
    math::vec3d target_vel, math::vec3d launcher_pos, math::vec3d launcher_vel, double bullet_vel)
{
  math::vec3d p_c = target_pos - launcher_pos;
  math::vec3d v_c = target_vel - launcher_vel;

  double a = bullet_vel*bullet_vel - v_c.x()*v_c.x() - v_c.y()*v_c.y() - v_c.z()*v_c.z();
  double b = - 2*v_c.x()*p_c.x() - 2*v_c.y()*p_c.y() - 2*v_c.x()*p_c.x();
  double c = - p_c.x()*p_c.x() - p_c.y()*p_c.y() - p_c.z()*p_c.z();

  double determ = b*b-4*a*c;
  double tm,tx;
  if (determ < 0) return -1.0f;
  else
  {
    double t1 = (-b + math::sqrt(determ)) / (2*a);
    double t2 = (-b - math::sqrt(determ)) / (2*a);
    tm = math::min(t1,t2);
    tx = math::max(t1,t2);
    if(tm>0) return tm;
    else if(tx>0) return tx;
    else return -1.0f;
  }
}

math::vec3d SpaceElement::updatePosDir(math::vec3d &dst_pos, math::vec3d &dst_dir,
    const math::vec3d &tgt_dir, float max_speed, float acc,
    float lat_acc, float applyratio)
{
  math::vec3d udir = math::normalize(tgt_dir);
  dst_dir += udir*acc*applyratio + math::normalize(dst_dir)*(1.0f-applyratio)*acc;
  //dst_dir += math::normalize(dst_dir)*acc;

  double len = math::length(dst_dir);
  if(len > max_speed)
  {
    dst_dir *= max_speed / len; 
  }

  // check if the course is incorrect, correct with lateral acc amount when it is
  math::vec3d corrdir = dst_dir - static_cast<double>(max_speed)*udir;
  float corr_amount = static_cast<float>(math::length(corrdir));
  if(corr_amount < lat_acc)
  {
    lat_acc = corr_amount;
  }
  dst_dir += math::normalize(corrdir) * lat_acc * applyratio;

  dst_pos += dst_dir * game->getTimestep();

  return udir;
}
