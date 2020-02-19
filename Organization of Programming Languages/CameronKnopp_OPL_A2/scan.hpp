/* Definitions the scanner shares with the parser
    Michael L. Scott, 2008-2017.
*/
#ifndef SCAN_HPP
#define SCAN_HPP

typedef enum {t_read, t_write, t_id, t_literal, t_gets,
                t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof} token;

char token_image[100];

token scan();

#endif
