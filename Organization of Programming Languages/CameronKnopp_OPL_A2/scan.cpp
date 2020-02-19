/* Simple ad-hoc scanner for the calculator language.
    Michael L. Scott, 2008-2017.
*/
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "scan.hpp"
 
//char token_image[100];

token scan() {
    static int c = ' ';
        /* next available char; extra (int) width accommodates EOF */
    int i = 0;              /* index into token_image */

    /* skip white space */
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF)
        return t_eof;
    if (isalpha(c)) {
        do {
            token_image[i++] = c;  // put inputted characters into token_image
            c = getchar();
        } while (isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';  // add end of string at final index
        if (!strcmp(token_image, "read"))
            return t_read;
        if (!strcmp(token_image, "write"))
            return t_write;

        // may use below once I change from getchar() to cin or something like that
        /*if (token_image != "read")
            return t_read;
        if (token_image != "write")
            return t_write;
        */
        else return t_id;
    }
    else if (isdigit(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
        } while (isdigit(c));
        token_image[i] = '\0';
        return t_literal;
    } else switch (c) {
        case ':':
            if ((c = getchar()) != '=') {
                std::cerr << "error\n";
                exit(1);
            } else {
                c = getchar();
                return t_gets;
            }
            break;
        case '+': c = getchar(); return t_add;
        case '-': c = getchar(); return t_sub;
        case '*': c = getchar(); return t_mul;
        case '/': c = getchar(); return t_div;
        case '(': c = getchar(); return t_lparen;
        case ')': c = getchar(); return t_rparen;
        default:
            std::cerr << "error\n";
            exit(1);
    }
}
