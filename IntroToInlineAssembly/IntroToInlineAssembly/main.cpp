/*
Title: Intro to Inline Assembly
File Name: main.cpp
Copyright © 2016
Original authors: Luna Meier
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
This example is meant to show off how to work with inline assembly in
Visual Studios.  It's also an intro to how to work with the assembly language
in general.  This tutorial will not teach specifics on how to compile
assembly code externally, or how to call externals assembly functions.
This tutorial is solely meant for someone interested in having a better
understanding of what's going on under the hood.

References:
http://www.cs.virginia.edu/~evans/cs216/guides/x86.html //Basic assembly guide
https://msdn.microsoft.com/en-us/library/4ks26t93.aspx //Inline visual studio assembly information
http://www.felixcloutier.com/x86/ //x86 instruction set reference
*/

#include <iostream>

float test(int input);

int main(){

	// Before we begin let's talk about the tools at your disposal.
	// Hopefully at this point you know how to make a breakpoint in
	// visual studios, because understanding what is going on will
	// be a lot easier if you can see it.
	
	// While the program is stopped and in debug mode you get a host
	// of new options available to you to work with code.  There are a
	// few especially relevant ones for working with assembly.

	//  Set a break point here on this int initialization, and run the
	// program in debug mode.
	int b = 7;
	float f;

	// Alright, let's set up a few windows so you can see what I see.
	// Start by going to DEBUG->Windows->Disassembly
	// This is what your code is actually turning into under the hood.
	// So you'll see that "mov dword ptr[b], 7" is setting b to 7.
	//
	// Don't worry about what that specifically means right now; for now we'll
	// setup some more windows.  I suggest putting the dissasembly in its own
	// section so you can see your code clearly and the disassembly at the same time.
	// It's also important to note that if you click on the disassembly window, step
	// into and step over now move through the assembly instructions instead of the 
	// bigger code.
	//
	// The next thing you want to do is go to DEBUG->Windows->Registers
	//
	// You see, under the hood your cpu temporarily stores data in these registers.
	// When your computer is doing operations, it's usually on these registers.
	// The registers you see to start are the basic ones, but you actually have
	// access to more than those.  Right click in the registers window and turn on the
	// view of the floating point registers and the flags registers.  The others are useful
	// to watch too, but I won't be going over those registers in this tutorial.
	//
	// Now you can see in that window all of the math that your computer is doing.
	// 
	// Now there's one more set of windows you might want to look at.
	// At this point if you're on a single monitor things are getting a little cramped,
	// so I like to keep this one in a different tab on my actual code's window.  That way
	// When I'm reading the disassembly I can see this one or my actual code.
	// Go to DEBUG->Windows->Memory and pick any of the memory windows.  The purpose of
	// having four of them is just so you can look at four locations in memory at once.
	// 
	// Note that you can right click this window to change some properties.  I personally
	// like setting the ansi text to no text and making everything show as 4 byte integers,
	// just let's me read through everything a little quicker.  Now looking at your actual
	// memory is a little intimidating, but don't worry.  I'll explain how everything works
	// there later on.  Hide that window for now, we won't need it for a bit.


	// Now that we've setup our debugging tools let's start writing some basic assembly.
	// To write inline assembly in visual studio it has to be written inside of an asm
	// block.  __asm{ } is how you write the code.
	__asm{

		// A few things to note about this inline assembly.  Now that we're
		// in the world of assembly things work a little differently.
		//
		// First off, YOUR COMPILER WILL NOT OPTIMIZE CODE IN ASM BLOCKS.
		// When you tell the computer how your asm works it follows how your asm works.
		// it does not play safe for you.  Which means suddenly there's a lot of burden
		// on you to know what works in what way to write effective assembly code.
		//
		// Let's not worry about that for now though.  I'll link a good resource and
		// talk about some basic ones to consider later.
		//
		// Secondly, the code you write in an ASM block may not run if you were to try
		// to compile it to assembly in a separate assembly file.  That's because
		// visual studio does a bunch of really cool things to make it easier for
		// us to write inline assembly.
		//
		// For example, if I want to access that int b I declared before,
		// all I have to do is type b.  It makes keeping track of local variables
		// really easy.
		//
		// If I want to write comments, I can just use //.  Normally comments
		// are written after semi-colons.

		//; See ? this will compile. <-uncomment this to check.  It messes with my formatting.

		// Which is kind of weird, but it's also picking up the this keyword,
		// and it's unwieldy.  So just use // while you're doing inline assembly.
		//
		// Additionally, you CAN call functions inside of an asm block.  There's
		// just a couple of catches:
		// A.) Calling a function in assembly isn't the same as calling a function
		// in C or C++.  The way you pass in parameters and return requires you to
		// know how functions work under the hood.  We'll talk about that.
		//
		// B.) You can call C functions.
		//
		// C.) You can only call GLOBAL C++ functions, that don't have optional
		// parameters.  You CAN however call any functions declared with extern "C"

		// Okay, so that's a lot to keep in mind.  That's basically it though, aside
		// from that the rest of this is just standard assembly.

		// So before we write our first line of assembly we should talk about registers.
		// You have 8 32 bit registers that you can access.
		//
		// EAX, EBX, ECX, EDX, ESI, EDI, ESP, and EBP.
		// Now, unless you know explicitly what you're doing do not touch
		// ESP and EBP.  ESP is the stack pointer, and EBP is the base pointer.
		// Those registers are used so the program knows where it is in memory.
		//
		// The EIP register is the instruction register.  It contains the address of the
		// next instruction.  You can't change it directly, nor would you want to.
		// You can use jumps and calls to automatically change it.
		//
		// The EFL register is the flag register.  It's used for comparisons.
		// You won't have to change it yourself, and in most cases you have
		// absolutely no need to touch it.
		//
		// Out of EAX, EBX, ECX, EDX, EDI, and ESI, everything is more or less the same
		// These are the registers you do your operations in.  It's important to note
		// that there are a few differences.  All of the EAX, EBX, ECX, and EDX registers
		// allow you to access their lower WORD on its own with AX, BX, CX, and DX.  You can
		// access the lower BYTE of that WORD with AL, BL, CL, and DL.  You can also acccess
		// the higher BYTE of the low WORD with AH, BH, CH, and DH.  EDI and ESI have no
		// such function
		//
		// It's also useful to note that EAX is sometimes called the accumulator.  You don't
		// have to use it as your accumulator but it's worth noting anyway.
		// Additionally the data in the EAX register is what is returned at the end of a function.
		//
		// We'll show off how that works later.
		//
		// Alright, enough with the information, let's actually write some assembly.

		mov eax, b;
		// So the mov instruction is a fairly simple one.  It takes data from one
		// location and copies it to another.
		// If I wanted to store the contents of eax to b, I'd write
		mov b, eax;
		// You'll want to note that the first operand is always the destination
		// operand, where the data is going to be stored after an operation.
		// the second is called the source operand, because it's where the data is coming from.
		// If you watch your registers, you'll also note that eax is still 7.  Again, it copies.
		mov ebx, 8;
		// We can also directly store values into these registers.
		
		// A neat trick that is a minor optimization in general, is when you want to
		// set a register to zero, you don't go mov eax, 0.
		
		xor ebx, ebx;
		// Instead you use the xor operation, which as you would expect, xor's the two operands.
		// It works faster because it's a smaller instruction in terms of opcode bytes.
		// This means small instruction size, and better cacheability.
		// If you don't understand what that means, don't worry about it and move on.
		
		// Next let's talk about load effective address.
		// It loads the address of data into memory.  So let's do that with b into ecx.

		lea ecx, b;

		// Now the address of b is stored in ecx.  Useful when you don't want to load data
		// just a pointer to it.
		
		// Now let's access the memory stored in ecx.
		mov ebx, DWORD ptr[ecx];
		// You'll notice we specify what kind of data is being pulled out.
		// You can specify DWORD, WORD, or BYTE.  This is important so the processor
		// knows what to grab.
		//
		// ptr[register] is how you acces a specific data.
		mov DWORD ptr[ecx], ebx;
		// Is also totally legal.
		
		// You can also add together for shifting if you want to do pointer math.
		// [ecx + edx] is totally legal.  So is [ecx + edx * 4] if you want to scale the
		// data access.

		// If you want to increment or decrement a register by one, then we can use inc
		// or dec to do so.

		inc ebx;

		// If we want to add two int's together, we can do so with ADD;

		add eax, ebx; //stores into eax.

		// If we want to subtract two numbers, use sub.

		sub eax, ebx; //eax = eax - ebx;		

		// Okay so integer division and multiplication is a little different.
		// For multiplication, store the number you're multiplying into eax.
		// eax will be your destination operand, you can't specify it.

		// Division is a little more complicated.
		// You store the number you're dividing into eax.
		// then you divide it by the operand.
		// the quotient is stored into EAX, and the remainder is stored in EDX.

		// If we want to multiply two numbers, use mul.

		mul ebx; //stores into eax.

		// Let's increment by one before we show off integer division.
		inc eax;

		// For division, div.
		div ebx;

		// Right now, eax should be 7, ebx is still 8, and edx is 1.  Check for yourself in the registers to
		// watch it happen.  Remember that everything is in hex.

		// Alright, let's talk about two more useful operations.  Push and Pop.

		// If you want to locally store some data on the stack, you can do so with push and pop.
		// generally, when you have an asm code block you want to define any variables you're going to be
		// using at the top of the function before the asm block, so you have easy words to reference thanks to
		// the inline assembly features.  Additionally, it allocates the memory in a better manner.
		
		// Now, when you see the following code watch what happens to the esp register.

		push eax;
		inc eax;
		pop eax;

		// It subtracts four to make space for the local variable.  In fact, if you know
		// very explicitly the layout of your data, you can actually access local variables
		// by subtracting the right amount away from ebp, your base pointer.

		// Now we'll do a compare.
		// Does 6 == 8?  Pretty sure it doesn't.
		dec eax;

		// First let's make a label
	TopOfLoop:
		// This is how you write a label in assembly.  The code can jump to labels locally.
		
		// Now we want to loop until eax == ebx.
		inc eax;

		// So first, compare eax with ebx
		cmp eax, ebx; // This will change the flag register to describe the two variables.
		// We're not actually storing anything there.
		// Next jump IFF the two of those were equal.

		jne TopOfLoop;//je is jump if not equal to the label listed.  Go to http://www.felixcloutier.com/x86/Jcc.html to see all the different conditionals.
		// It's a little bloated of a resource, but it explains them pretty well.

		// Cool, we made it past the loop.  Now we're going to cover function calls and floating point math.
		// Let's start with a function call.

		// We're going to call test, and pass eax in.
		// To pass a parameter into a function, you push that parameter,
		// You start with the furthest out parameter, and work backwards.
		// There's only one parameter here so we'll just pass in eax as
		// our int.

		// It's important to know that you don't know if code will change the registers you're working with.
		// So if you want to protect a specific register, push it before the function call, and then pop it
		// afterwards.  I'm not concerned about any of my registers right now, so I'll just pass in a parameter.

		push eax;
		// Then you call the function.
		call test;
		
		// Okay, before you read any more here go inside the function and read that first.
		// The tutorial on how floating point numbers work is there.
		//
		// The first thing we have to do is clear the stack of the parameters we passed in.
		// we passed in a single 4 byte parameter, so we need to move the stack pointer.
		add esp, 4;
		
		// There!  Cleaned up our mess.  

		// Okay, now to get the value out of the return, since it was returning a float
		// we know for a fact that it'll be stored in ST(0).

		fst f;
		
		// And that's it!  If you check your locals that weird cosine result is stored in f.

		// If you want to learn more assembly, read: http://www.cs.virginia.edu/~evans/cs216/guides/x86.html
		// first and then look at what all of the x86 instructions are here: http://www.felixcloutier.com/x86/
		//
		// I highly recommend looking at all sorts of code in disassembly it's cool to see how 
		// a lot of common functions are programmed.
		
		// If you're concerned about optimization, go to http://www.bit.ly/programgames and go to the optimization section.
		// In it there are going to be a number of additional assembly guides, explaining how to write SSE instructions
		// in assembly code, as well as how to write optimized assembly code.  It's important to learn at least the latter,
		// as your inline code won't be optimized by the compiler.  If you're writing inline assembly, I'm going to go ahead
		// and assume it's to make your code /run faster/.  It won't run faster if you don't know what you're doing though.

		// But for just reading assembly code and writing it?  You should have a basic understanding at this point.
	}


	return 1;
}

float test(int input){

	float a = 1.0f;
	float b = 1.5f;

	__asm{
		// Alright!  Time to talk about floats.
		// You have eight registers to work with just floats.  ST(0) through ST(7).
		// A lot of the float operations act on ST(0), sort of like div worked on eax.

		// To load a float we use FLD.
		fld a;
		fld b;
		// You'll note that unlike the integer registers we were working with, these are
		// a stack that you push onto.
		// You can also push one of the stacks.
		fld st(1);//This should push a a second time, since a should be at ST(1);

		// Now, when you're adding or subtracting you have to use the ST(0) register
		// You however can choose whether you're storing to ST(0) or ST(i) where i is/
		// some number.

		fadd st(0), st(1); //stores in st(0);
		fadd st(1), st(0);; //stores in st(1);

		fsub st(0), st(1); //sub goes st(0) = st(0) - st(1);
		fsubr st(0), st(1); //subr let's you go st(0) = st(1) - st(0);

		// Mul and div are obvious from here.  They work the same as add.
		fmul st(0), st(1); 
		fdiv st(0), st(1);

		// You can also do cosine and sine!  They work on ST(0);
		fcos;
		fsin;
		
		// There's even an operation that does sine and cosine of a number at the same time,
		// stores sine(st(0)) in st(0), and then pushes cos(st(0)).
		fsincos;

		// Now, notice how there's no return at the bottom of this function?

		// Since there's an asm block and the compiler is smart, it can tell that
		// we said we're returning a float and there's definitely work being done on
		// the st(0) register.  So whatever we want to return is just left in st(0).
		// We don't have to write any more assembly here now, since this is a C++
		// function, and not an actual assembly file.
	}

}