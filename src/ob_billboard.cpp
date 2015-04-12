#include "ob_billboard.hpp"

#include "gfx/surface.hpp"
#include "gfx/texture_2d.hpp"
#include "math/random.hpp"

using namespace ob;

Billboard::Billboard(const gfx::Texture2D *tex) :
  m_billboard(tex),
  m_billboard_disabled(false) { }

void Billboard::enableRandom(int level)
{
  m_billboard_disabled = (math::mrand(0, level) == 0);
}

const gfx::Texture2D* Billboard::updateBillboard(BillboardArray &parray,
    const gfx::Shader &sh, const gfx::Texture2D *tex) const
{
  // If same, just return.
  if(tex == m_billboard)
  {
    return tex;
  }

  // Flush if tex is something.
  if(NULL != tex)
  {
    parray.feed(sh, *tex);
  }

  // Start anew with billboards of this.
  parray.clear();
  return m_billboard;
}

