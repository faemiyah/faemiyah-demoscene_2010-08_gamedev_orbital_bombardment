#ifndef GFX_IMAGE_JPEG_HPP
#define GFX_IMAGE_JPEG_HPP

#include "defaults.hpp"

namespace gfx
{
  /** Tell if given bit depth allowed by JPEG format.
   *
   * \param op Bit depth. */
  inline bool image_jpeg_supports_bpp(unsigned op)
  {
    return ((op == 8) || (op == 24));
  }

  /** \brief Probe JPEG file.
   *
   * \param pfname Filename to probe.
   * \param Required to be a volume?
   * \return Bit depth found.
   */
  extern unsigned image_jpeg_probe(const std::string &filename, bool require_volume = false);

  /** \brief Read a JPEG image.
   *
   * The destination parameters are left unaltered on failure. They will only
   * be written on success.
   *
   * Throws an error on failure.
   *
   * \param pw Destination width.
   * \param ph Destination height.
   * \param pb Destiantion bit depth.
   * \param pdata Destination data.
   * \param filename Source filename.
   * \param required_bpp If set, require this bit depth.
   */
  extern void image_jpeg_load(unsigned &pw, unsigned &ph, unsigned &pb, uint8_t *&pdata,
      const std::string &filename, unsigned required_bpp = 0);

  /** \brief Read an 'extended' JPEG image with depth axis.
   *
   * The destination parameters are left unaltered on failure. They will only
   * be written on success.
   *
   * Throws an error on failure.
   *
   * \param pw Destination width.
   * \param ph Destination height.
   * \param pd Destination depth.
   * \param pb Destiantion bit depth.
   * \param pdata Destination data.
   * \param filename Source filename.
   * \param required_bpp If set, require this bit depth.
   */
  extern void image_jpeg_load_extended(unsigned &pw, unsigned &ph, unsigned &pb, unsigned &pd, uint8_t *&pdata,
      const std::string &filename, unsigned required_bpp = 0);

  /** \brief Save a JPEG image.
   *
   * Throws an error on failure.
   *
   * \param filename Destination filename.
   * \param pw Source width.
   * \param ph Source height.
   * \param pb Source bit depth.
   * \param pdata Source data.
   * \param pquality Quality to save at, (default = 92).
   */
  extern void image_jpeg_save(const std::string &filename, unsigned pw, unsigned ph, unsigned pb,
      uint8_t *pdata, int pquality = 92);

  /** \brief Save an 'extended' JPEG image with depth axis.
   *
   * Throws an error on failure.
   *
   * \param filename Destination filename.
   * \param pw Source width.
   * \param ph Source height.
   * \param pd Source depth.
   * \param pb Source bit depth.
   * \param pdata Source data.
   * \param pquality Quality to save at, (default = 92).
   */
  extern void image_jpeg_save_extended(const std::string &filename, unsigned pw, unsigned ph, unsigned pd,
      unsigned pb, uint8_t *pdata, int pquality = 92);
};

#endif
