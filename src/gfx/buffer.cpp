#include "gfx/buffer.hpp"

#include "gfx/lod.hpp"
#include "thr/dispatch.hpp"

using namespace gfx;

namespace gfx
{
  template<> void BufferElem::upload(Lod &lod)
  {
    std::vector<Lod*> lods;
    lod.collectAll(lods);

    unsigned idx = 0;
    std::vector<Triangle> triangles;
    BOOST_FOREACH(Lod *vv, lods)
    {
      vv->assignElem(idx);
      BOOST_FOREACH(const Triangle &tt, vv->getFaces())
      {
        triangles.push_back(tt);
        idx += 3 * static_cast<unsigned>(sizeof(GLuint));
      }
    }

    this->upload(triangles);
  }
}

static void buffer_unreserve(GLuint *op)
{
  if(0 != *op)
  {
    glDeleteBuffers(1, op);
    *op = 0;
  }
}

void gfx::buffer_unreserve_dispatch(GLuint *op)
{
  thr::wait_privileged(&buffer_unreserve, op);
}

void BufferInterleavedRWTCNV::bind(const Attribute &rr, const Attribute &ww, const Attribute &tt,
    const Attribute &cc, const Attribute &nn, const Attribute &vv) const
{
  int32_t *offset = NULL;

  this->Buffer<GL_ARRAY_BUFFER>::bind();

  enable_buffers(rr, ww, tt, cc, nn, vv);

  glVertexAttribPointer(rr.id(), 4, GL_UNSIGNED_BYTE, GL_FALSE, BLOCKSIZE, offset);
  offset += 1;

  glVertexAttribPointer(ww.id(), 4, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 4;

  glVertexAttribPointer(tt.id(), 2, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 2;

  glVertexAttribPointer(cc.id(), 4, GL_UNSIGNED_BYTE, GL_TRUE, BLOCKSIZE, offset);
  offset += 1;

  glVertexAttribPointer(nn.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 3;

  glVertexAttribPointer(vv.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
}

void BufferInterleavedRWTCNV::upload(const std::vector<math::vec4u> &rr, const std::vector<math::vec4f> &ww,
    const std::vector<math::vec2f> &tt, const std::vector<Color> &cc, const std::vector<math::vec3f> &nn,
    const std::vector<math::vec3f> &vv)
{
  unsigned vcnt = static_cast<unsigned>(vv.size());
  int32_t *ildata = new int32_t[BLOCKSIZE / sizeof(int32_t) * vcnt],
          *iter = ildata;

  for(unsigned ii = 0; (ii < vcnt); ++ii)
  {
    uint8_t *rptr = reinterpret_cast<uint8_t*>(iter);
    rptr[0] = static_cast<uint8_t>(rr[ii].x());
    rptr[1] = static_cast<uint8_t>(rr[ii].y());
    rptr[2] = static_cast<uint8_t>(rr[ii].z());
    rptr[3] = static_cast<uint8_t>(rr[ii].w());
    iter += 1;

    float *wptr = reinterpret_cast<float*>(iter);
    wptr[0] = ww[ii].x();
    wptr[1] = ww[ii].y();
    wptr[2] = ww[ii].z();
    wptr[3] = ww[ii].w();
    iter += 4;

    float *tptr = reinterpret_cast<float*>(iter);
    tptr[0] = tt[ii].x();
    tptr[1] = tt[ii].y();
    iter += 2;

    uint8_t *cptr = reinterpret_cast<uint8_t*>(iter);
    cptr[0] = static_cast<uint8_t>(math::lround(cc[ii].r() * 255.0f));
    cptr[1] = static_cast<uint8_t>(math::lround(cc[ii].g() * 255.0f));
    cptr[2] = static_cast<uint8_t>(math::lround(cc[ii].b() * 255.0f));
    cptr[3] = static_cast<uint8_t>(math::lround(cc[ii].a() * 255.0f));
    iter += 1;

    float *nptr = reinterpret_cast<float*>(iter);
    nptr[0] = nn[ii].x();
    nptr[1] = nn[ii].y();
    nptr[2] = nn[ii].z();
    iter += 3;

    float *vptr = reinterpret_cast<float*>(iter);
    vptr[0] = vv[ii].x();
    vptr[1] = vv[ii].y();
    vptr[2] = vv[ii].z();
    iter += 3;
  }

  this->reserve();
  this->Buffer<GL_ARRAY_BUFFER>::bind();
  glBufferData(get_buffer_type(), BLOCKSIZE * vcnt, ildata, GL_STATIC_DRAW);
  delete[] ildata;
}

void BufferInterleavedTCNV::bind(const Attribute &tt, const Attribute &cc, const Attribute &nn,
    const Attribute &vv) const
{
  int32_t *offset = NULL;

  this->Buffer<GL_ARRAY_BUFFER>::bind();

  enable_buffers(tt, cc, nn, vv);

  glVertexAttribPointer(tt.id(), 2, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 2;

  glVertexAttribPointer(cc.id(), 4, GL_UNSIGNED_BYTE, GL_TRUE, BLOCKSIZE, offset);
  offset += 1;

  glVertexAttribPointer(nn.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 3;

  glVertexAttribPointer(vv.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
}

void BufferInterleavedTCNV::upload(const std::vector<math::vec2f> &tt, const std::vector<Color> &cc,
    const std::vector<math::vec3f> &nn, const std::vector<math::vec3f> &vv)
{
  unsigned vcnt = static_cast<unsigned>(vv.size());
  int32_t *ildata = new int32_t[BLOCKSIZE / sizeof(int32_t) * vcnt],
          *iter = ildata;

  for(unsigned ii = 0; (ii < vcnt); ++ii)
  {
    float *tptr = reinterpret_cast<float*>(iter);
    tptr[0] = tt[ii].x();
    tptr[1] = tt[ii].y();
    iter += 2;

    uint8_t *cptr = reinterpret_cast<uint8_t*>(iter);
    cptr[0] = static_cast<uint8_t>(math::lround(cc[ii].r() * 255.0f));
    cptr[1] = static_cast<uint8_t>(math::lround(cc[ii].g() * 255.0f));
    cptr[2] = static_cast<uint8_t>(math::lround(cc[ii].b() * 255.0f));
    cptr[3] = static_cast<uint8_t>(math::lround(cc[ii].a() * 255.0f));
    iter += 1;

    float *nptr = reinterpret_cast<float*>(iter);
    nptr[0] = nn[ii].x();
    nptr[1] = nn[ii].y();
    nptr[2] = nn[ii].z();
    iter += 3;

    float *vptr = reinterpret_cast<float*>(iter);
    vptr[0] = vv[ii].x();
    vptr[1] = vv[ii].y();
    vptr[2] = vv[ii].z();
    iter += 3;
  }

  this->reserve();
  this->Buffer<GL_ARRAY_BUFFER>::bind();
  glBufferData(get_buffer_type(), BLOCKSIZE * vcnt, ildata, GL_STATIC_DRAW);
  delete[] ildata;
}

void BufferInterleavedCNV::bind(const Attribute &cc, const Attribute &nn, const Attribute &vv) const
{
  int32_t *offset = NULL;

  this->Buffer<GL_ARRAY_BUFFER>::bind();

  enable_buffers(cc, nn, vv);

  glVertexAttribPointer(cc.id(), 4, GL_UNSIGNED_BYTE, GL_TRUE, BLOCKSIZE, offset);
  offset += 1;

  glVertexAttribPointer(nn.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 3;

  glVertexAttribPointer(vv.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
}

void BufferInterleavedCNV::upload(const std::vector<Color> &cc, const std::vector<math::vec3f> &nn,
    const std::vector<math::vec3f> &vv)
{
  unsigned vcnt = static_cast<unsigned>(vv.size());
  int32_t *ildata = new int32_t[BLOCKSIZE / sizeof(int32_t) * vcnt],
          *iter = ildata;

  for(unsigned ii = 0; (ii < vcnt); ++ii)
  {
    uint8_t *cptr = reinterpret_cast<uint8_t*>(iter);
    cptr[0] = static_cast<uint8_t>(math::lround(cc[ii].r() * 255.0f));
    cptr[1] = static_cast<uint8_t>(math::lround(cc[ii].g() * 255.0f));
    cptr[2] = static_cast<uint8_t>(math::lround(cc[ii].b() * 255.0f));
    cptr[3] = static_cast<uint8_t>(math::lround(cc[ii].a() * 255.0f));
    iter += 1;

    float *nptr = reinterpret_cast<float*>(iter);
    nptr[0] = nn[ii].x();
    nptr[1] = nn[ii].y();
    nptr[2] = nn[ii].z();
    iter += 3;

    float *vptr = reinterpret_cast<float*>(iter);
    vptr[0] = vv[ii].x();
    vptr[1] = vv[ii].y();
    vptr[2] = vv[ii].z();
    iter += 3;
  }

  this->reserve();
  this->Buffer<GL_ARRAY_BUFFER>::bind();
  glBufferData(get_buffer_type(), BLOCKSIZE * vcnt, ildata, GL_STATIC_DRAW);
  delete[] ildata;
}

void BufferInterleavedTCV::bind(const Attribute &tt, const Attribute &cc, const Attribute &vv) const
{
  int32_t *offset = NULL;

  this->Buffer<GL_ARRAY_BUFFER>::bind();

  enable_buffers(tt, cc, vv);

  glVertexAttribPointer(tt.id(), 2, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 2;

  glVertexAttribPointer(cc.id(), 4, GL_UNSIGNED_BYTE, GL_TRUE, BLOCKSIZE, offset);
  offset += 1;

  glVertexAttribPointer(vv.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
}

void BufferInterleavedTCV::upload(const std::vector<math::vec2f> &tt, const std::vector<Color> &cc,
    const std::vector<math::vec3f> &vv)
{
  unsigned vcnt = static_cast<unsigned>(vv.size());
  int32_t *ildata = new int32_t[BLOCKSIZE / sizeof(int32_t) * vcnt],
          *iter = ildata;

  for(unsigned ii = 0; (ii < vcnt); ++ii)
  {
    float *tptr = reinterpret_cast<float*>(iter);
    tptr[0] = tt[ii].x();
    tptr[1] = tt[ii].y();
    iter += 2;

    uint8_t *cptr = reinterpret_cast<uint8_t*>(iter);
    cptr[0] = static_cast<uint8_t>(math::lround(cc[ii].r() * 255.0f));
    cptr[1] = static_cast<uint8_t>(math::lround(cc[ii].g() * 255.0f));
    cptr[2] = static_cast<uint8_t>(math::lround(cc[ii].b() * 255.0f));
    cptr[3] = static_cast<uint8_t>(math::lround(cc[ii].a() * 255.0f));
    iter += 1;

    float *vptr = reinterpret_cast<float*>(iter);
    vptr[0] = vv[ii].x();
    vptr[1] = vv[ii].y();
    vptr[2] = vv[ii].z();
    iter += 3;
  }

  this->reserve();
  this->Buffer<GL_ARRAY_BUFFER>::bind();
  glBufferData(get_buffer_type(), BLOCKSIZE * vcnt, ildata, GL_STATIC_DRAW);
  delete[] ildata;
}

void BufferInterleavedTV::bind(const Attribute &tt, const Attribute &vv) const
{
  int32_t *offset = NULL;

  this->Buffer<GL_ARRAY_BUFFER>::bind();

  enable_buffers(tt, vv);

  glVertexAttribPointer(tt.id(), 2, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
  offset += 2;

  glVertexAttribPointer(vv.id(), 3, GL_FLOAT, GL_FALSE, BLOCKSIZE, offset);
}

void BufferInterleavedTV::upload(const std::vector<math::vec2f> &tt, const std::vector<math::vec3f> &vv)
{
  unsigned vcnt = static_cast<unsigned>(vv.size());
  int32_t *ildata = new int32_t[BLOCKSIZE / sizeof(int32_t) * vcnt],
          *iter = ildata;

  for(unsigned ii = 0; (ii < vcnt); ++ii)
  {
    float *tptr = reinterpret_cast<float*>(iter);
    tptr[0] = tt[ii].x();
    tptr[1] = tt[ii].y();
    iter += 2;

    float *vptr = reinterpret_cast<float*>(iter);
    vptr[0] = vv[ii].x();
    vptr[1] = vv[ii].y();
    vptr[2] = vv[ii].z();
    iter += 3;
  }

  this->reserve();
  this->Buffer<GL_ARRAY_BUFFER>::bind();
  glBufferData(get_buffer_type(), BLOCKSIZE * vcnt, ildata, GL_STATIC_DRAW);
  delete[] ildata;
}

void gfx::enable_buffers(const Attribute &attr1)
{
  BOOST_ASSERT(attr1.id() <= 0);

  attr1.enable();

  vertex_attribute_array_disable(1);
  vertex_attribute_array_disable(2);
  vertex_attribute_array_disable(3);
  vertex_attribute_array_disable(4);
  vertex_attribute_array_disable(5);
}

void gfx::enable_buffers(const Attribute &attr1, const Attribute &attr2)
{
  BOOST_ASSERT(attr1.id() <= 1);
  BOOST_ASSERT(attr2.id() <= 1);
  BOOST_ASSERT(attr1.id() != attr2.id());

  attr1.enable();
  attr2.enable();

  vertex_attribute_array_disable(2);
  vertex_attribute_array_disable(3);
  vertex_attribute_array_disable(4);
  vertex_attribute_array_disable(5);
}

void gfx::enable_buffers(const Attribute &attr1, const Attribute &attr2,
    const Attribute &attr3)
{
  BOOST_ASSERT(attr1.id() <= 2);
  BOOST_ASSERT(attr2.id() <= 2);
  BOOST_ASSERT(attr3.id() <= 2);
  BOOST_ASSERT(attr1.id() != attr2.id());
  BOOST_ASSERT(attr1.id() != attr3.id());
  BOOST_ASSERT(attr2.id() != attr3.id());

  attr1.enable();
  attr2.enable();
  attr3.enable();

  vertex_attribute_array_disable(3);
  vertex_attribute_array_disable(4);
  vertex_attribute_array_disable(5);
}

void gfx::enable_buffers(const Attribute &attr1, const Attribute &attr2,
    const Attribute &attr3, const Attribute &attr4)
{
  BOOST_ASSERT(attr1.id() <= 3);
  BOOST_ASSERT(attr2.id() <= 3);
  BOOST_ASSERT(attr3.id() <= 3);
  BOOST_ASSERT(attr4.id() <= 3);
  BOOST_ASSERT(attr1.id() != attr2.id());
  BOOST_ASSERT(attr1.id() != attr3.id());
  BOOST_ASSERT(attr1.id() != attr4.id());
  BOOST_ASSERT(attr2.id() != attr3.id());
  BOOST_ASSERT(attr2.id() != attr4.id());
  BOOST_ASSERT(attr3.id() != attr4.id());

  attr1.enable();
  attr2.enable();
  attr3.enable();
  attr4.enable();

  vertex_attribute_array_disable(4);
  vertex_attribute_array_disable(5);
}

void gfx::enable_buffers(const Attribute &attr1, const Attribute &attr2,
    const Attribute &attr3, const Attribute &attr4, const Attribute &attr5)
{
  BOOST_ASSERT(attr1.id() <= 4);
  BOOST_ASSERT(attr2.id() <= 4);
  BOOST_ASSERT(attr3.id() <= 4);
  BOOST_ASSERT(attr4.id() <= 4);
  BOOST_ASSERT(attr5.id() <= 4);
  BOOST_ASSERT(attr1.id() != attr2.id());
  BOOST_ASSERT(attr1.id() != attr3.id());
  BOOST_ASSERT(attr1.id() != attr4.id());
  BOOST_ASSERT(attr1.id() != attr5.id());
  BOOST_ASSERT(attr2.id() != attr3.id());
  BOOST_ASSERT(attr2.id() != attr4.id());
  BOOST_ASSERT(attr2.id() != attr5.id());
  BOOST_ASSERT(attr3.id() != attr4.id());
  BOOST_ASSERT(attr3.id() != attr5.id());
  BOOST_ASSERT(attr4.id() != attr5.id());

  attr1.enable();
  attr2.enable();
  attr3.enable();
  attr4.enable();
  attr5.enable();

  vertex_attribute_array_disable(5);
}

void gfx::enable_buffers(const Attribute &attr1, const Attribute &attr2,
    const Attribute &attr3, const Attribute &attr4, const Attribute &attr5,
    const Attribute &attr6)
{
  BOOST_ASSERT(attr1.id() <= 5);
  BOOST_ASSERT(attr2.id() <= 5);
  BOOST_ASSERT(attr3.id() <= 5);
  BOOST_ASSERT(attr4.id() <= 5);
  BOOST_ASSERT(attr5.id() <= 5);
  BOOST_ASSERT(attr6.id() <= 5);
  BOOST_ASSERT(attr1.id() != attr2.id());
  BOOST_ASSERT(attr1.id() != attr3.id());
  BOOST_ASSERT(attr1.id() != attr4.id());
  BOOST_ASSERT(attr1.id() != attr5.id());
  BOOST_ASSERT(attr1.id() != attr6.id());
  BOOST_ASSERT(attr2.id() != attr3.id());
  BOOST_ASSERT(attr2.id() != attr4.id());
  BOOST_ASSERT(attr2.id() != attr5.id());
  BOOST_ASSERT(attr2.id() != attr6.id());
  BOOST_ASSERT(attr3.id() != attr4.id());
  BOOST_ASSERT(attr3.id() != attr5.id());
  BOOST_ASSERT(attr3.id() != attr6.id());
  BOOST_ASSERT(attr4.id() != attr5.id());
  BOOST_ASSERT(attr4.id() != attr6.id());
  BOOST_ASSERT(attr5.id() != attr6.id());

  attr1.enable();
  attr2.enable();
  attr3.enable();
  attr4.enable();
  attr5.enable();
  attr6.enable();
}

