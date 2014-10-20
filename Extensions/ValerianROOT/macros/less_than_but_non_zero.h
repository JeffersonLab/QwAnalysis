/********************************************
 *  * \author <b>Programmer:<\b> Valerie Gray
 * \author <b>Assisted By:<\b>
 *
 * \brief <b>Purpose:</b> This is header file
 *      less then but not zero function function.
 *
 *      As it is used by all the macros that
 *      will be a part of Valerian Root this is
 *      the best way to deal with that
 *
 * \date <b>Date:</b> 10-08-2014
 * \date <b>Modified:</b>
 *
 * \note <b>Entry Conditions:</b> none
 *
********************************************/

/***********************************************************
 Function: less_than_but_non_zero
 Purpose:
 To find the lowest value in an array/vector that is non zero
 by forcing zero to be infinity

 helper function that turns any zero value into a +infinity
 (zero is larger than any other number)

 if x== 0 no matter what y is this is FALSE, since x is
 zero it can't be less then y

 if y == 0, no matter what x is this is TRUE since x is not zero
 it is less than and not zero

 else compare x < y, if x is then TRUE else FALSE

 Entry Conditions:
 - Double_t x
 - Double_t y

 Global:
 none

 Exit Conditions: bool
 Called By: Make_graphs
 Date: 06-06-2014
 Modified:
 Brilliance by: Wouter Deconinck
 *********************************************************/
bool less_than_but_non_zero(Double_t x, Double_t y)
{
  if (x == 0)
    return false;
  if (y == 0)
    return true;
  return (x < y);
}
