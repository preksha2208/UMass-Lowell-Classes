/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2017.
*/

#include <iostream>
#include <string>
#include <exception>
#include "stdio.h"
#include "stdlib.h"
#include "scan.h"

const char *names[] = {"check", "read", "write", "id", "literal", "gets", "if",
                       "fi", "do", "od", "equal", "notequal", "smaller",
                       "greater", "smallerorequal", "greaterorequal", "add", "sub", "mul",
                       "div", "lparen", "rparen", "eof", "eps"};

static token input_token;
std::string img;

std::string program();
std::string stmt_list();
std::string stmt();
std::string expr();
std::string expr_tail();
std::string term_tail();
std::string term();
std::string factor_tail();
std::string factor();
std::string relation_op();
std::string add_op();
std::string mul_op();
std::string relation();

void error()
{ //error function
    std::cout << "Syntax error" << std::endl;
    exit(1);
}

bool find(token array[], token item)
{
    for (int i = 0; i < sizeof(*array) / sizeof(array[0]); i++)
    {
        if (item == array[i])
        {
            return true;
        }
    }
    return false;
}

std::string match(token expected)
{
    if (input_token == expected)
    {
        img = (" \"" + updatedImage() + "\"");
        input_token = scan();
    }
    else
    {
        img = (" \"" + updatedImage() + "\"");
        throw std::string("error");
    }
    return "";
}

std::string program()
{
    try
    {
        switch (input_token)
        {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_do:
        case t_check:
        case t_eof:
        {
            std::string str = "(program \n";
            str.append("[");
            str.append(stmt_list());
            match(t_eof);
            str.append("]\n");
            return (str.append(")\n"));
        }
        default:
            throw std::string("program");
            return "";
        }
    }
    catch (std::string e)
    {
        std::cout << "syntax error\n";
        return "";
    }
}

std::string stmt_list()
{
    switch (input_token)
    {
    case t_id:
    case t_check:
    case t_write:
    case t_read:
    case t_if:
    case t_do:
    {
        std::string str = "";
        str.append("(" + stmt());
        str.append(stmt_list());
        return str.append(")\n");
    }
    case t_eof:
        return "\n"; /*  epsilon production */
    default:
        return "\n";
    }
}

std::string stmt()
{
    try
    {
        switch (input_token)
        {
        case t_id:
        {
            match(t_id);
            match(t_gets);
            std::string str = "( := (id " + img + ")";
            str.append(relation());
            str.append(")");
            return str;
        }
        case t_read:
            match(t_read);
            match(t_id);
            return ("read (id" + img + " )\n");
        case t_write:
        {
            match(t_write);
            std::string str = relation();
            return ("(write " + str + ")\n");
        }
        case t_if:
        {
            match(t_if);
            std::string first = "(if \n";
            first.append(relation());
            std::string second = stmt_list();
            match(t_fi);
            return first + "[\n" + second + "])\n";
        }
        case t_do:
        {
            match(t_do);
            std::string str = "(do\n";
            str.append(stmt_list());
            match(t_od);
            return "[" + str + "])\n";
        }
        case t_check:
        {
            match(t_check);
            std::string str = "";
            str.append(relation());
            return "(check\n" + str + ")\n";
        }
        default:
            error();
            return "";
        }
    }
    catch (std::string error)
    {
        token follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof};
        while (input_token != t_eof && find(follow, input_token) == false)
        {
            input_token = scan();
        }
        if (find(follow, input_token))
            return "syntax error\n";
    }

    return "";
}

std::string expr()
{
    try
    {
        std::string first = term();
        std::string second = term_tail();

        if (second != "")
        {
            for (int i = 0; i < second.length(); ++i)
            {
                if (second[i] == ' ')
                {
                    return (second.substr(0, i) + " " + first + " " + second.substr(i + 1, second.length() - i));
                }
            }
        }
        else
        {
            return first;
        }
    }
    catch (std::string it)
    {
        token follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen, t_equal, t_notequal, t_smaller, t_greater, t_smallerorequal, t_greaterorequal};
        while (input_token != t_eof && find(follow, input_token) == false)
        {
            input_token = scan();
        }
        if (find(follow, input_token))
        {
            return "syntax error\n";
        }
        else
        {
            return "";
        }
    }
    error();
    return "";
}

std::string expr_tail()
{

    switch (input_token)
    {
    case t_equal:
    case t_notequal:
    case t_smaller:
    case t_greater:
    case t_smallerorequal:
    case t_greaterorequal:
    {
        std::string first = relation_op();
        std::string second = expr();
        first.append(" ");
        first.append(second);
        return first;
    }
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
        return "";
    default:
        return "";
    }
}

std::string term_tail()
{
    switch (input_token)
    {
    case t_add:
    case t_sub:
    {
        std::string first = add_op();
        first.append(" ");
        first.append(term());
        std::string second = term_tail();
        if (second != "")
        {
            for (int i = 0; i < second.length(); ++i)
            {
                if (second[i] == ' ')
                {
                    return (second.substr(0, i) + " " + first + " " + second.substr(i + 1, second.length() - i));
                }
            }
        }
        else
        {
            return first;
        }

        return "error";
    }
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
        return ""; /*  epsilon production */
    default:
        return "";
    }
}

std::string term()
{
    try
    {
        std::string first = factor();
        std::string second = factor_tail();
        if (second != "")
        {
            for (int i = 0; i < second.length(); ++i)
            {
                if (second[i] == ' ')
                {
                    return (second.substr(0, i) + " " + first + " " + second.substr(i + 1, second.length() - i));
                }
            }
        }
        else
        {
            return first;
        }
    }
    catch (std::string error)
    {
        throw std::string("error");
    }
    return "";
}

std::string factor_tail()
{
    switch (input_token)
    {
    case t_mul:
    case t_div:
    {
        std::string first = mul_op();
        std::string second = factor();
        first.append(second);
        return (first.append(factor_tail()));
    }
    case t_add:
    case t_sub:
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
        return ""; /*  epsilon production */
    default:
        return "";
    }
}

std::string factor()
{

    switch (input_token)
    {
    case t_id:
    {
        match(t_id);
        return ("(id" + img + ")");
    }
    case t_literal:
    {
        match(t_literal);
        return ("(lit" + img + ")");
    }
    case t_lparen:
    {
        match(t_lparen);
        std::string rel = relation();
        match(t_rparen);
        return "(" + rel + ")";
    }
    default:
        throw std::string("error");
        return "";
    }
}

std::string relation_op()
{
    switch (input_token)
    {
    case t_equal:
        match(t_equal);
        return "== ";

    case t_notequal:
        match(t_notequal);
        return "<> ";

    case t_smaller:
        match(t_smaller);
        return "< ";

    case t_greater:
        match(t_greater);
        return "> ";

    case t_smallerorequal:
        match(t_smallerorequal);
        return "<= ";

    case t_greaterorequal:
        match(t_greaterorequal);
        return ">= ";

    default:
        throw std::string("error");
        return "";
    }
}

std::string add_op()
{
    switch (input_token)
    {
    case t_add:
        match(t_add);
        return "+ ";

    case t_sub:
        match(t_sub);
        return "- ";

    default:
        throw std::string("error");
        return "";
    }
}

std::string mul_op()
{
    switch (input_token)
    {
    case t_mul:
        match(t_mul);
        return "* ";

    case t_div:
        match(t_div);
        return "/ ";

    default:
        throw std::string("error");
        return "";
    }
}

std::string relation()
{
    try
    {
        std::string first = expr();
        std::string second = expr_tail();
        first = "(" + first;

        if (second != "")
        {
            for (int i = 0; i < second.length(); ++i)
            {
                if (second[i] == ' ')
                {
                    return second.substr(0, i) + " " + first + " " + second.substr(i + 1, second.length() - i) + ")\n";
                }
            }
        }
        else
        {
            return (first.append(")\n"));
        }
    }
    catch (std::string error)
    {
        token follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen};

        while (input_token != t_eof && find(follow, input_token) == false)
        {
            input_token = scan();
        }
        if (find(follow, input_token))
        {
            return "syntax error\n";
        }
        return " eof";
    }
    return "";
}

int main()
{
    input_token = scan();
    std::string finalResult = program();
    std::cout << finalResult;
    return 0;
}
