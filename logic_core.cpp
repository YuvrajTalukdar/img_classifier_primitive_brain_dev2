#include"logic_core.h"

using namespace std;

void logic_core::set_logic_core_no(int no)
{
    if(locked==false)
    {   logic_core_no=no;}
    else
    {   cout<<"\nERROR failed to set logic_core_no as it is already locked";}
}

string logic_core::return_core_name(int index)
{   return available_names[index];}

int logic_core::return_no_of_nn_cores_needed()
{   return no_of_nn_cores_needed;}

logic_core::logic_core()
{
    available_names.push_back("RED");
    available_names.push_back("GREEN");
    available_names.push_back("BLUE");
    available_names.push_back("CANNY_EDGE");
}