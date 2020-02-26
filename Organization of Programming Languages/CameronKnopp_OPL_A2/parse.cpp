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

const char *names[] = {"check", "read", "write", "id", "literal", "gets", "if",
                       "fi", "do", "od", "equal", "notequal", "smaller",
                       "greater", "smallerorequal", "greaterorequal", "add", "sub", "mul",
                       "div", "lparen", "rparen", "eof", "eps"};

const std::string fullNames[] = {"\"check\"", "\"read\"", "\"write\"", "id", "literal", "\"gets\"", "\"if\"",
                                 "\"fi\"", "\"do\"", "\"od\"", "\"==\"", "\"<>\"", "\"<\"", "\">\"", "\"<=\"", "\">=\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"",
                                 "\"(\"", "\")\"", "\"eof\"", "\"eps\""};

static token input_token;
static int tabNum = 0; //tab spaces
static int hasError = 0;
static std::string image = "";
// check for the follow sets of statement, relation and expression to handle exceptions catched
static token s_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof};
static token r_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen};
static token e_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen, t_equal, t_notequal, t_smaller, t_greater, t_smallerorequal, t_greaterorequal};
//////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////

void error()
{ //error function
    std::cout << "Syntax error" << std::endl;
    exit(1);
}

std::string match(token expected)
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
        std::cout << "Token image: " << names[input_token] << ", Token expected:  " << fullNames[expected] << std::endl;
        throw std::string("match");
    }
    return "";
}

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

std::string program()
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

            std::string str1 = "(program \n";
            str1 += "[";

            str1 += stmt_list();
            match(t_eof);
            str1 += "]\n";
            return str1 + ")\n";
        }
        default:
            throw std::string("program"); // default case
            return "";
        }
    }
    catch (std::string e)
    { // Once you catch it
        std::cout << "Not expecting " << names[input_token] << " in Program" << std::endl;
        return "";
    }
}

std::string stmt_list()
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
        std::string str1 = "";

        str1 += "(" + stmt();
        str1 += stmt_list();

        str1 += ")\n";

        return str1;
    }
    case t_eof:

        return "\n"; /*  epsilon production */
    default:

        return "\n";
    }
}

std::string stmt()
{ // STATEMENT

    try
    { // using try/catch
        switch (input_token)
        { // using switch/case
        case t_id:
        {
            match(t_id);
            match(t_gets);
            std::string str1 = "( := (id " + image + ")" + relation();

            str1 += ")";

            return str1;
        }
        case t_read:
            match(t_read);
            match(t_id);

            return "read (id" + image + " )\n";
        case t_write:
        {
            match(t_write);
            std::string str1 = relation();

            return "(write " + str1 + ")\n";
        }
        case t_if:
        {
            match(t_if);
            std::string str1 = "(if \n";

            str1 += relation();

            std::string str2 = stmt_list();

            match(t_fi);

            return str1 + "[\n" + str2 + "])\n";
        }
        case t_do:
        {
            match(t_do);
            std::string str1 = "(do\n";
            str1 += stmt_list();

            match(t_od);

            return "[" + str1 + "])\n";
        }
        case t_check:
        {
            match(t_check);
            std::string str1 = "";

            str1 += relation();

            return "(check\n" + str1 + ")\n";
        }
        default:
            error();

            return "";
        }
    }
    catch (std::string it)
    { // Once you catch it
        if (it == "match")
            std::cout << "Not expecting " << fullNames[input_token] << " in statement" << std::endl;
        else
            std::cout << "Not expecting " << fullNames[input_token] << " in " << it << std::endl;
        std::cout << "Skipped: " << fullNames[input_token] << std::endl;
        input_token = scan();
        while (!contains(input_token, s_follow) && input_token != t_eof)
        {
            std::cout << "Skipped: " << fullNames[input_token] << std::endl;
            input_token = scan();
        }
        if (contains(input_token, s_follow))
        {
            return "(error)\n";
        }
        else
        {
        }
        return "";
    }
}

std::string expr()
{

    try
    { // using try/catch exception
        std::string str1 = term();
        std::string str2 = term_tail();

        return prefix(str1, str2);
    }
    catch (std::string it)
    {
        if (it == "match")
            std::cout << "Not expecting " << fullNames[input_token] << " in expression" << std::endl;
        else
            std::cout << "Not expecting " << fullNames[input_token] << " in " << it << std::endl;
        std::cout << "Skipped: " << fullNames[input_token] << std::endl;
        input_token = scan();
        while (!contains(input_token, e_follow) && input_token != t_eof)
        {
            std::cout << "Skipped: " << fullNames[input_token] << std::endl;
            input_token = scan();
        }
        if (contains(input_token, e_follow))
        {
            return "(error)\n";
        }
        else
        {
        }

        return "";
    }
    error();

    return "";
}

std::string expr_tail()
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
        std::string str1 = relation_op();
        std::string str2 = expr();

        return str1 + " " + str2;
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
    { //using swith/case
    case t_add:
    case t_sub:
    {
        std::string str1 = add_op();
        str1 += " ";
        str1 += term();
        std::string str2 = term_tail();

        return prefix(str1, str2);
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
    { // using try/catch exception

        std::string str1 = factor();
        std::string str2 = factor_tail();

        return prefix(str1, str2);
    }
    catch (std::string it)
    { // once you catch it
        throw std::string("term");
    }
    return "";
}

std::string factor_tail()
{

    switch (input_token)
    { //using switch/case
    case t_mul:
    case t_div:
    {
        std::string str1 = mul_op();
        std::string str2 = factor();
        str1 += str2;
        str1 += factor_tail();

        return str1 + "";
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
    { // using switch/case
    case t_id:
    {
        match(t_id);

        std::string str1 = "(id" + image + ")";
        return str1;
    }
    case t_literal:
    {
        match(t_literal);

        std::string str1 = "(lit" + image + ")";
        return str1;
    }
    case t_lparen:
    {
        match(t_lparen);
        std::string str1 = relation();
        match(t_rparen);

        return "(" + str1 + ")";
    }
    default:
        throw std::string("factor"); //default case

        return "";
    }
}

std::string relation_op()
{ // RELATION FUNCTION

    switch (input_token)
    { // using switch/case
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
        throw std::string("relation_op");

        return "";
    }
}

std::string add_op()
{

    switch (input_token)
    { // using switch/case
    case t_add:
        match(t_add);

        return "+ ";
    case t_sub:
        match(t_sub);

        return "- ";
    default:
        throw std::string("add_op");

        return "";
    }
}

std::string mul_op()
{

    switch (input_token)
    { // using switch/case
    case t_mul:
        match(t_mul);

        return "* ";
    case t_div:
        match(t_div);

        return "/ ";
    default:

        throw std::string("mul_op");
        return "";
    }
}

std::string relation()
{ //modified relation
    try
    { // using try/catch exception

        std::string str2 = expr();
        std::string str1 = expr_tail();

        return "(" + prefix(str2, str1) + ")\n";
    }
    catch (std::string it)
    { // once you catch it
        if (it == "match")
            std::cout << "Not expecting " << fullNames[input_token] << " in relation" << std::endl;
        else
            std::cout << "Not expecting " << fullNames[input_token] << " in " << it << std::endl;
        std::cout << "Skipped: " << fullNames[input_token] << std::endl;
        input_token = scan();
        while (!contains(input_token, r_follow) && input_token != t_eof)
        {
            std::cout << "Skipped: " << fullNames[input_token] << std::endl;
            input_token = scan();
            std::cout << input_token << std::endl;
        }
        if (contains(input_token, r_follow))
        {
            return "(error)\n";
        }
        else
        {
        }

        return " eof";
    }
}

int main()
{                           // LETS CALL THE PROGRAM
    input_token = scan();   // take the input
    std::cout << program(); // program does its thing
    return 0;
}
