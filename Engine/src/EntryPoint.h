#include "Application/Application.h"
/*
  TODO:
  * remove GLCall for every gl function
  * remove all "changes meaning errors"
  * block|pool allocator
  * windows support && test it
  * ECS
  * Engine Editor

*/


extern void create_user_application();

int main(int argc, char** argv)
{
    create_user_application();
    application_start();
    return 0;
}
