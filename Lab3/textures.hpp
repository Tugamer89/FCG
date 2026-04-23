#ifndef EMBED_HH
#define EMBED_HH

#include <cstddef>

/* * Utilizzo di C++26 #embed per importare direttamente
 * i file binari all'interno dell'eseguibile a tempo di compilazione.
 */

constexpr unsigned char paddle_png[] = {
#embed "../resources/texture_paddle.png"
};
constexpr std::size_t paddle_png_len = sizeof(paddle_png);

constexpr unsigned char ball_png[] = {
#embed "../resources/texture_ball.png"
};
constexpr std::size_t ball_png_len = sizeof(ball_png);

#endif
