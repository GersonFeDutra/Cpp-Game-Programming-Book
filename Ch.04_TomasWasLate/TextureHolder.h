#pragma once
#ifndef TEXTURE_HOLDER_H
#define TEXTURE_HOLDER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
using namespace sf;

class TextureHolder {
private:
  // A map container from the STL
  // that holds related pairs of String and Texture
  std::map<std::string, Texture> m_Textures;

  // A pointer of the same type as the class itself
  // the one and only instance
  static TextureHolder *m_s_Instance;

public:
  TextureHolder(); // must be called once
  static Texture &GetTexture(std::string const &filename);
};
#endif
