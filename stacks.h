// C implementation of the data stack and functions to manipulate it.

#pragma once
extern int STACK[];
extern int SP;

int pop();
int peek();
void push(int);
