#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/26/2024
Allrights reserved by David Ge

********************************************************************/

/*
call it after using thread pool and no longer need the thread pool, for example, at the end of a program
*/
void removeThreadPool();

unsigned int getAvailableThreadCount();
unsigned int getThreadCount();