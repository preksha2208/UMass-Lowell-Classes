/* Simple ad-hoc scanner for the calculator language.
    Michael L. Scott, 2008-2017.
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <ios>
#include "scan.h"

char token_image[100];
std::string image;
std::string updatedImage()
{
    return image;
}

token scan()
{
    static char c = ' ';
    /* next available char; extra (int) width accommodates EOF */
    int i = 0; /* index into token_image */

    /* skip white space */

    do
    {
        if (!std::cin.get(c))
            return t_eof;
    } while (isspace(c));
    if (isalpha(c))
    {
        do
        {
            token_image[i++] = c;
            if (!std::cin.get(c))
                break;
        } while (isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';
        if (token_image == std::string("read"))
            return t_read;
        else if (token_image == std::string("write"))
            return t_write;
        else if (token_image == std::string("do"))
            return t_do;
        else if (token_image == std::string("od"))
            return t_od;
        else if (token_image == std::string("if"))
            return t_if;
        else if (token_image == std::string("fi"))
            return t_fi;
        else if (token_image == std::string("check"))
            return t_check;
        else
        {
            image = token_image;
            return t_id;
        }
    }
    else if (isdigit(c))
    {
        do
        {
            token_image[i++] = c;
            std::cin.get(c);
        } while (isdigit(c));
        token_image[i] = '\0';
        image = token_image;
        return t_literal;
    }
    else
        switch (c)
        { // using switch/case
        case '+':
            std::cin.get(c);
            return t_add;
        case '-':
            std::cin.get(c);
            return t_sub;
        case '*':
            std::cin.get(c);
            return t_mul;
        case '/':
            std::cin.get(c);
            return t_div;
        case '(':
            std::cin.get(c);
            return t_lparen;
        case ')':
            std::cin.get(c);
            return t_rparen;
        case ':':
            std::cin.get(c);
            if (c == '=')
            {
                std::cin.get(c);
                return t_gets;
            }
            std::cout << "error\n";
        case '<':
            std::cin.get(c);
            if (c == '>')
            {
                std::cin.get(c);
                return t_notequal;
            }
            else if (c == '=')
            {
                std::cin.get(c);
                return t_smallerorequal;
            }
            else
                return t_smaller;
        case '>':
            std::cin.get(c);
            if (c == '=')
            {
                std::cin.get(c);
                return t_greaterorequal;
            }
            else
                return t_greater;
        case '=':
            std::cin.get(c);
            if (c == '=')
            {
                std::cin.get(c);
                return t_equal;
            }
            std::cout << "error\n";
        default:
            std::cout << "error\n";
            exit(1);
        }
}
