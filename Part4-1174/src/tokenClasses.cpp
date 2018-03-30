//============================================================================
//
//% Student Name 1: student1
//% Student 1 #: 123456781
//% Student 1 userid (email): stu1 (stu1@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put NONE if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put �none�)__
//
// Also, list any resources beyond the course textbook and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Enter the above information in tokenClasses.hpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P4_<userid1>_<userid2>" (eg. P4_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClasses.cpp
// Description : Course Project Part 4
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#include "tokenClasses.hpp"
//#include <memory>

// I'm not sure if next line works with other than GNU compiler
#include <cxxabi.h>

using namespace std;

namespace ensc251 {

Token* int_assignment_exp();
Token* shift_exp();
Token* int_div_exp();

unsigned int tracker = 0; // tracker index
std::vector<Token*> tokenObjectPs; // objects for abstract syntax tree

char *demangle(const char *typeName) {
	int status;
	// I'm not sure if the below line works with compilers other than GNU
	return abi::__cxa_demangle(typeName, 0, 0, &status);
}

Token* postfix_exp()
{
	int old_tracker = tracker;
	Token* lowerNodeP;
	Token* upperNodeP;
	if((upperNodeP = tokenObjectPs[tracker]->process_id()))
	{
		if((lowerNodeP = tokenObjectPs[tracker]->process_postfix_operator()))
		{
			upperNodeP->add_childP(lowerNodeP);
			lowerNodeP = upperNodeP;
			return lowerNodeP;
		}
	}

	tracker = old_tracker;
	auto subTreeP = tokenObjectPs[tracker]->process_primary_exp();
	if(!subTreeP)
		return nullptr;
	return subTreeP;
}

Token* int_postfix_exp()
{
	int old_tracker = tracker;
	Token* lowerNodeP;
	Token* upperNodeP;
	if((upperNodeP = tokenObjectPs[tracker]->process_int_id()))
	{
		if((lowerNodeP = tokenObjectPs[tracker]->process_postfix_operator()))
		{
			upperNodeP->add_childP(lowerNodeP);
			lowerNodeP = upperNodeP;
			return lowerNodeP;
		}
	}

	tracker = old_tracker;
	auto subTreeP = tokenObjectPs[tracker]->process_int_primary_exp();
	if(!subTreeP)
		return nullptr;
	return subTreeP;
}

Token* comp_exp()
{
	int old_tracker = tracker;

	Token* tObjP;

	if(!(tObjP = tokenObjectPs[tracker]->process_int_comp()))
	{
		tracker = old_tracker;
		tObjP = postfix_exp();
	}
	return tObjP;
}

Token* int_comp_exp()
{
	int old_tracker = tracker;

	Token* tObjP;

	if(!(tObjP = tokenObjectPs[tracker]->process_int_comp()))
	{
		tracker = old_tracker;
		tObjP = int_postfix_exp();
	}
	return tObjP;
}

Token* div_exp()
{
	int old_tracker = tracker;

	Token* ueTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = comp_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()))
		{
			if(!(ueTreeP = comp_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(ueTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	else
	{
		tracker = old_tracker;
		lowerNodeP = int_div_exp();
	}

	return lowerNodeP;
}

Token* int_div_exp()
{
	Token* ueTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = int_comp_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()) || (upperNodeP = tokenObjectPs[tracker]->advance_past_mod_operator()))
		{
			if(!(ueTreeP = int_comp_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(ueTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* additive_exp(){
	Token* subTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	if ((lowerNodeP = div_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_additive_operator()))
		{
			if(!(subTreeP = div_exp()))
					return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(subTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* int_additive_exp(){
	Token* subTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	if ((lowerNodeP = int_div_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_additive_operator()))
		{
			if(!(subTreeP = int_div_exp()))
					return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(subTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* shift_exp()
{
	Token* subTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;

	if ((lowerNodeP = int_additive_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_shift_operator()))
		{
			if(!(subTreeP = int_additive_exp()))
					return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(subTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* ternary_exp()
{
	int old_tracker = tracker;
	Token* subTreeP;

	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->advance_past_conditional_operator()))
		{
			tObjP->add_childP(subTreeP);
			subTreeP = assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					subTreeP = ternary_exp();
					if(subTreeP)
					{
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}

	tracker = old_tracker;
	subTreeP = shift_exp();

	if(!subTreeP)
	{
		tracker = old_tracker;
		subTreeP = additive_exp();
		if(!subTreeP)
			return nullptr;
	}
	return subTreeP;
}

Token* int_ternary_exp()
{
	int old_tracker = tracker;
	Token* subTreeP;

	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->advance_past_conditional_operator()))
		{
			tObjP->add_childP(subTreeP);
			subTreeP = int_assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					subTreeP = int_ternary_exp();
					if(subTreeP)
					{
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}
	tracker = old_tracker;
	subTreeP = shift_exp();
	if(!subTreeP)
		return nullptr;
	return subTreeP;
}

Token* assignment_exp()
{
	int old_tracker = tracker;

	// int_id int_assignment_operator int_assignment_exp
	Token* subTreeP = tokenObjectPs[tracker]->process_int_id();
	if(subTreeP)
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_int_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	// production: id gen_assignment_operator assignment_exp
	tracker = old_tracker;
	if ((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_gen_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}

		if(tokenObjectPs[tracker]->has_string_value(";"))
			// we have already built a unary expression subtree, which satisfies the ternary_exp production.
			return subTreeP;
	}
	tracker = old_tracker;
	subTreeP = ternary_exp();
	if(subTreeP)
		return subTreeP;
	else
		return nullptr;
}

Token* int_assignment_exp()
{
	int old_tracker = tracker;

	Token* subTreeP = tokenObjectPs[tracker]->process_int_id();
	if(subTreeP)
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_gen_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}

		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_int_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = int_assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				return nullptr;
		}
	}

	tracker = old_tracker;
	subTreeP = int_ternary_exp();
	if(subTreeP)
		return subTreeP;
	else
		return nullptr;
}

Token* stat()
{
	if(auto subTreeP = tokenObjectPs[tracker]->process_declaration())
		return subTreeP;
	else
		return assignment_exp();
}

Token* stat_list()
{
	Token* statTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = stat())) {

		while (tokenObjectPs[tracker]->has_string_value(";")) {
			tracker++;
			if (tracker == tokenObjectPs.size())
				return lowerNodeP;
			upperNodeP = tokenObjectPs[tracker - 1];

			if(!(statTreeP = stat()))
				throw ensc251::TokenException(tokenObjectPs[tracker]);

			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(statTreeP);
			lowerNodeP = upperNodeP; //because we are returning lowerNodeP
		}
	}
	throw ensc251::TokenException(tokenObjectPs[tracker]);
}

Token* recursive_parser()
{
	if (tokenObjectPs.size())
		return stat_list();
	else
		throw ensc251::TokenException();
}
}

