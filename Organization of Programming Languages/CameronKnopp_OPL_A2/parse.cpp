/* Complete recursive descent parser for the calculator language.
    Builds on figure 2.16.  Prints a trace of productions predicted and
    tokens matched.  Does no error recovery: prints "syntax error" and
    dies on invalid input.
    Michael L. Scott, 2008-2017.
*/

#include <iostream>
#include <string>
#include "stdio.h"
#include "stdlib.h"
#include "scan.h"

const std::string names[] = {"check", "read", "write", "id", "literal", "gets", "if",
                             "fi", "do", "od", "equal", "notequal", "smaller",
                             "greater", "smallerorequal", "greaterorequal", "add", "sub", "mul",
                             "div", "lparen", "rparen", "eof", "eps"};

const std::string fullNames[] = {"\"check\"", "\"read\"", "\"write\"", "id", "literal", "\"gets\"", "\"if\"",
                                 "\"fi\"", "\"do\"", "\"od\"", "\"==\"", "\"<>\"", "\"<\"", "\">\"", "\"<=\"", "\">=\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"",
                                 "\"(\"", "\")\"", "\"eof\"", "\"eps\""};

static token s_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof};
static token r_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen};
static token e_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen, t_equal, t_notequal, t_smaller, t_greater, t_smallerorequal, t_greaterorequal};

static token input_token;
static int hasError = 0;
static std::string image = "";

std::string prefix(std::string str, std::string tail)
{ // Print Prefix
    if (tail == "")
        return str;
    for (int i = 0; i < tail.length(); ++i)
    {
        if (tail[i] == ' ')
        {
            return tail.substr(0, i) + " " + str + " " + tail.substr(i + 1, tail.length() - i);
        }
    }
    return "prefix error"; //Print the error message
}
std::string preIndent(std::string str, int tab)
{ // Print PreIndent
    for (int i = 0; i <= tab; i++)
    {
        str = " " + str;
    }
    return str;
}
int contains(token take, token set[])
{ // CHECK if is in the set[] array
    int i = 0;
    while (set[i])
    {
        if (take == set[i++])
        {
            return 1;
        }
    }
    return 0;
}
std::string postIndent(std::string str, int tab)
{ // Print postIndent
    for (int i = 0; i <= tab; i++)
    {
        str += " ";
    }
    return str;
}
////////////////////////////////////////////////////////////////////////////////////////

void error()
{ //error function
    std::cout << "Syntax error" << std::endl;
    exit(1);
}

void match(token expected)
{ // Match function
    if (input_token == expected)
    {
        image = getImage();
        input_token = scan();
        // std::cout << "Matched "<<names[expected] << " image: "<<image<< std::endl;
        //std::cout << "match next: "<<names[input_token] << std::endl;
    }
    else
    {
        image = getImage();
        std::cout << "Token " << names[input_token] << ": do you mean " << fullNames[expected] << "?" << std::endl;
        throw std::string("match");
    }
    return;
}

void program();
void stmt_list();
void stmt();
void expr();
void expr_tail();
void term_tail();
void term();
void factor_tail();
void factor();
void relation_op();
void relation();
void add_op();
void mul_op();

void program()
{
    try
    { // try/catch exception
        switch (input_token)
        { // does its thing with input
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_do:
        case t_check:
        case t_eof:
        {
            std::cout << "(program \n";

            std::cout << "[";

            stmt_list();
            match(t_eof);

            std::cout << "]\n";

            if (hasError)
                return;
            std::cout << ")\n";
            return;
        }
        default:
            throw std::string("program"); // default case
            return;
        }
    }
    catch (std::string e)
    { // Once you catch it
        std::cout << " Not expecting " << names[input_token] << " in Program" << std::endl;
        return;
    }
}

void stmt_list()
{
    switch (input_token)
    { // using switch case
    case t_id:
    case t_check:
    case t_write:
    case t_read:
    case t_if:
    case t_do:
    { // add if/do
        std::cout << "(";
        stmt();
        stmt_list();

        std::cout << ")\n";

        return;
    }
    case t_eof:

        std::cout << "\n"; /*  epsilon production */
        return;
    default:

        std::cout << "\n";
        return;
    }
}

void stmt()
{ // STATEMENT

    try
    { // using try/catch
        switch (input_token)
        { // using switch/case
        case t_id:
        {
            match(t_id);
            match(t_gets);
            std::cout << "( := (id " << image << ")";
            relation();

            std::cout << ")";

            return;
        }
        case t_read:
            match(t_read);
            match(t_id);

            std::cout << "read (id" << image << " )\n";
            return;
        case t_write:
        {
            match(t_write);
            relation();

            std::cout << "(write ";
            relation();
            std::cout << ")\n";
            return;
        }
        case t_if:
        {
            match(t_if);
            std::cout << "(if \n";

            match(t_fi);

            relation();
            std::cout << "[\n";
            stmt_list();
            std::cout << "])\n";
            return;
        }
        case t_do:
        {
            match(t_do);
            std::cout << "(do\n";

            match(t_od);

            std::cout << "[";
            stmt_list();
            std::cout << "])\n";
        }
        case t_check:
        {
            match(t_check);

            std::cout << "(check\n";
            relation();
            std::cout << ")\n";
        }
        default:
            error();

            return;
        }
    }
    catch (std::string it)
    {                 // Once you catch it
        hasError = 1; // It has an error
        if (it == "match")
            std::cout << " Not expecting " << fullNames[input_token] << " in Statement" << std::endl;
        else
            std::cout << " Not expecting " << fullNames[input_token] << " in " << it << std::endl;
        std::cout << " Skipped: " << fullNames[input_token] << std::endl;
        input_token = scan();
        while (!contains(input_token, s_follow) && input_token != t_eof)
        {
            std::cout << " Skipped: " << fullNames[input_token] << std::endl;
            input_token = scan();
        }
        if (contains(input_token, s_follow))
        {
            hasError = 1;
            std::cout << "(error)\n";
            return;
        }
        else
        {
        }
        return;
    }
}

void expr()
{
    ;
    try
    { // using try/catch exception
        term();
        term_tail();

        return;
        //return prefix(str1, str2);
    }
    catch (std::string it)
    {
        hasError = 1;
        if (it == "match")
            std::cout << " Not expecting " << fullNames[input_token] << " in expression" << std::endl;
        else
            std::cout << " Not expecting " << fullNames[input_token] << " in " << it << std::endl;
        std::cout << " Skipped: " << fullNames[input_token] << std::endl;
        input_token = scan();
        while (!contains(input_token, e_follow) && input_token != t_eof)
        {
            std::cout << " Skipped: " << fullNames[input_token] << std::endl;
            input_token = scan();
        }
        if (contains(input_token, e_follow))
        {
            hasError = 1;

            std::cout << "(error)\n";
            return;
        }
        return;
    }
    error();

    return;
}

void expr_tail()
{
    switch (input_token)
    { //using switch/case
    case t_equal:
    case t_notequal:
    case t_smaller:
    case t_greater:
    case t_smallerorequal:
    case t_greaterorequal:
    {
        relation_op();
        std::cout << " ";
        expr();
        return;
    }
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
        return;
    default:
        return;
    }
}

void term_tail()
{
    switch (input_token)
    { //using swith/case
    case t_add:
    case t_sub:
    {
        add_op();
        std::cout << " ";
        term();
        term_tail();

        //return prefix(str1, str2);
        return;
    }
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
        return; /*  epsilon production */
    default:
        return;
    }
}

void term()
{
    try
    { // using try/catch exception
        ;
        factor();
        factor_tail();

        return;
    }
    catch (std::string it)
    { // once you catch it
        throw std::string("Term");
    }
    return;
}

void factor_tail()
{
    ;
    switch (input_token)
    { //using switch/case
    case t_mul:
    case t_div:
    {
        mul_op();
        factor();
        factor_tail();
        return;
    }
    case t_add:
    case t_sub:
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
        return; /*  epsilon production */
    default:
        return;
    }
}

void factor()
{
    ;
    switch (input_token)
    { // using switch/case
    case t_id:
    {
        match(t_id);
        std::cout << "(id" << image << ")";
        return;
    }
    case t_literal:
    {
        match(t_literal);
        std::cout << "(lit" << image << ")";
        return;
    }
    case t_lparen:
    {
        match(t_lparen);
        std::cout << "(";
        relation();
        match(t_rparen);
        std::cout << ")";
    }
    default:
        throw std::string("Factor"); //default case
        return;
    }
}

void relation_op()
{ // RELATION FUNCTION
    ;
    switch (input_token)
    { // using switch/case
    case t_equal:
        match(t_equal);
        std::cout << "== ";
        return;

    case t_notequal:
        match(t_notequal);
        std::cout << "<> ";
        return;

    case t_smaller:
        match(t_smaller);
        std::cout << "< ";
        return;

    case t_greater:
        match(t_greater);
        std::cout << "> ";
        return;

    case t_smallerorequal:
        match(t_smallerorequal);
        std::cout << "<= ";
        return;

    case t_greaterorequal:
        match(t_greaterorequal);
        std::cout << ">= ";
        return;

    default:
        throw std::string("Relation Operation");
        std::cout << "";
        return;
    }
}

void add_op()
{
    switch (input_token)
    { // using switch/case
    case t_add:
        match(t_add);
        std::cout << "+ ";
        return;
    case t_sub:
        match(t_sub);
        std::cout << "- ";
        return;
    default:
        throw std::string("Arithmetic Operator");
        return;
    }
}

void mul_op()
{
    switch (input_token)
    { // using switch/case
    case t_mul:
        match(t_mul);
        std::cout << "* ";
        return;
    case t_div:
        match(t_div);
        std::cout << "/ ";
        return;
    default:
        throw std::string("Multiplication Operator");
        std::cout << "";
        return;
    }
}

void relation()
{ //modified relation
    try
    { // using try/catch exception
        std::cout << "(";
        expr();
        expr_tail();
        std::cout << ")\n";
        // std::cout << "(" + prefix(str2, str1) + ")\n";
        return;
    }
    catch (std::string it)
    {                 // once you catch it
        hasError = 1; // has an error
        if (it == "match")
            std::cout << " Not expecting " << fullNames[input_token] << " in Relation" << std::endl;
        else
            std::cout << " Not expecting " << fullNames[input_token] << " in " << it << std::endl;
        std::cout << " Skipped: " << fullNames[input_token] << std::endl;
        input_token = scan();
        while (!contains(input_token, r_follow) && input_token != t_eof)
        {
            std::cout << " Skipped: " << fullNames[input_token] << std::endl;
            input_token = scan();
            std::cout << input_token << std::endl;
        }
        if (contains(input_token, r_follow))
        {
            hasError = 1;
            std::cout << "(error)\n";
            return;
        }
        else
        {
        }
        std::cout << " eof";
        return;
    }
}

int main()
{                        
    input_token = scan(); 
    program();           
    return 0;
}
