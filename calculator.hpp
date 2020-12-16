//
// Created by MSTRBRS on 15.12.2020.
//

#ifndef MOBILFONE_TEST_CALCULATOR_HPP
#define MOBILFONE_TEST_CALCULATOR_HPP

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

#if defined(_MSC_VER)
# pragma warning(disable: 4345)
#endif

#include <boost/blank.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <string>





namespace mobilfone
{
    namespace AST
    {
        /// <summary>
        /// Объявляем стандартные типы для нашего AST парсера
        /// </summary>
        struct _signed;
        struct program;

        // структура операнда
        typedef boost::variant<boost::blank,
            float,
            boost::recursive_wrapper<_signed>,
            boost::recursive_wrapper<program>> operand;

        struct _signed
        {
            char sign;
            operand _operand;
        };
        struct operation
        {
            char _operator;
            operand _operand;
        };
        struct program
        {
            operand first;
            std::list<operation> rest;
        };
    }
}
/// <summary>
/// Адаптация структур для парсинга через возможности Boost.Fusion
/// </summary>
BOOST_FUSION_ADAPT_STRUCT(
        mobilfone::AST::_signed,
        (char, sign)
        (mobilfone::AST::operand, _operand)
)
BOOST_FUSION_ADAPT_STRUCT(
        mobilfone::AST::operation,
        (char, _operator)
        (mobilfone::AST::operand, _operand)
)
BOOST_FUSION_ADAPT_STRUCT(
        mobilfone::AST::program,
        (mobilfone::AST::operand, first)
        (std::list<mobilfone::AST::operation>, rest)
)
namespace mobilfone
{
    namespace AST
    {
        struct printer
        {
            void operator()(boost::blank) const {}
            void operator()(float n) const { std::cout << n; }

            void operator()(operation const& x) const
            {
                boost::apply_visitor(*this, x._operand);
                switch (x._operator)
                {
                    case '+': std::cout << " add"; break;
                    case '-': std::cout << " subt"; break;
                    case '*': std::cout << " mult"; break;
                    case '/': std::cout << " div"; break;
                }
            }

            void operator()(_signed const& x) const
            {
                boost::apply_visitor(*this, x._operand);
                switch (x.sign)
                {
                    case '-': std::cout << " neg"; break;
                    case '+': std::cout << " pos"; break;
                }
            }

            void operator()(program const& x) const
            {
                boost::apply_visitor(*this, x.first);
                BOOST_FOREACH(operation const& oper, x.rest)
                            {
                                std::cout << ' ';
                                (*this)(oper);
                            }
            }
        };
        struct eval
        {
            typedef float result_type;

            float operator()(boost::blank) const;
            float operator()(float n) const { return n; }

            float operator()(operation const& x, float lhs) const
            {
                float rhs = boost::apply_visitor(*this, x._operand);
                switch (x._operator)
                {
                    case '+': return lhs + rhs;
                    case '-': return lhs - rhs;
                    case '*': return lhs * rhs;
                    case '/': return lhs / rhs;
                }
                BOOST_ASSERT(0);
                return 0;
            }

            float operator()(_signed const& x) const
            {
                float rhs = boost::apply_visitor(*this, x._operand);
                switch (x.sign)
                {
                    case '-': return -rhs;
                    case '+': return +rhs;
                }
                BOOST_ASSERT(0);
                return 0;
            }

            float operator()(program const& x) const
            {
                float state = boost::apply_visitor(*this, x.first);
                BOOST_FOREACH(operation const& oper, x.rest)
                            {
                                state = (*this)(oper, state);
                            }
                return state;
            }
        };

        float eval::operator()(boost::blank) const { BOOST_ASSERT(0); return 0; }
    }

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    template <typename Iterator>
    struct calculator_helper : qi::grammar<Iterator, AST::program(), ascii::space_type>
    {
        calculator_helper() : calculator_helper::base_type(expression)
        {
            qi::float_type float_;
            qi::char_type char_;

            expression = term >> *(   (char_('+') >> term) |   (char_('-') >> term));
            term = factor >> *(   (char_('*') >> factor) |   (char_('/') >> factor));
            factor = float_ |   '(' >> expression >> ')'  |   (char_('-') >> factor) |   (char_('+') >> factor);
        }

        qi::rule<Iterator, AST::program(), ascii::space_type> expression;
        qi::rule<Iterator, AST::program(), ascii::space_type> term;
        qi::rule<Iterator, AST::operand(), ascii::space_type> factor;
    };
    enum CALC_PARSE_STATUS : int
    {
        PARSE_OK,
        PARSE_ERROR,
    };
    class Calculator {
    private:
        typedef std::string::const_iterator iterator_type;
        typedef mobilfone::calculator_helper<iterator_type> calculator;
        typedef mobilfone::AST::program ast_program;
        typedef mobilfone::AST::printer ast_print;
        typedef mobilfone::AST::eval ast_eval;
    public:
        static CALC_PARSE_STATUS do_math(std::string expression, float& result)
        {
            calculator calc;
            ast_program program;
            ast_print print;
            ast_eval eval;

            std::string::const_iterator iter = expression.begin();
            std::string::const_iterator end = expression.end();
            boost::spirit::ascii::space_type space;
            bool parsed = phrase_parse(iter, end, calc, space, program);
            if (parsed && iter == end)
            {
                result = eval(program);
                return CALC_PARSE_STATUS::PARSE_OK;
            }
            else
                return CALC_PARSE_STATUS::PARSE_ERROR;
        }
    };
}


#endif //MOBILFONE_TEST_CALCULATOR_HPP
