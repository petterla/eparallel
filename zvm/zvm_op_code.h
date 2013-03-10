#ifndef ZVM_OP_CODE_H
#define ZVM_OP_CODE_H

namespace zvm{

	//load:load from local to register
	//push:push 
	//all op should clear the op_stack except load op(ILOAD/FLOAD/...)
	enum op_code{
		NULL_OP = 0,

		//load/store

		//ebx = eax
		AB,
		//ecx = eax
		AC,
		//edx = eax
		AD,
		BA,
		BC,
		BD,
		CA,
		CB,
		CD,
		DA,
		DB,
		DC,
		//push eax to op stack
		AOPSTACK,
		BOPSTACK,
		COPSTACK,
		DOPSTACK,
		//pop op stack to eax
		OPSTACKA,
		OPSTACKB,
		OPSTACKC,
		OPSTACKD,

		//set op_stack top be the const int/float
		//iconst (const int) 
		ICONST,
		//set eax be the const int/float
		ICONSTA,
		//set ebx be the const int/float
		ICONSTB,
		//set ecx be the const int/float
		ICONSTC,
		//set edx be the const int/float
		ICONSTD,


		//load local int/float to the op stack
		//iload (local index)
		ILOAD,
		//load local int/float to the eax
		ILOADA,
		//load local int/float to the ebx
		ILOADB,
		//load local int/float to the ecx
		ILOADC,
		//load local int/float to the edx
		ILOADD,


		//store op stack to local int
		//istore (local index)
		ISTORE,
		//store eax to local int/float
		ISTOREA,
		//store ebx to local int/float
		ISTOREB,
		//store ecx to local int/float
		ISTOREC,
		//store edx to local int/float
		ISTORED,

		//load int/float array element to op_stack
		//array is at eax, index at ebx
		//iaload ()
		IALOAD,
		//load int/float array element to eax
		IALOADA,
		//load int/float array element to ebx
		IALOADB,
		//load int/float array element to ecx
		IALOADC,
		//load int/float array element to edx
		IALOADD,

		//store op_stack to int/float array element
		//a is at eax, index at ebx
		//iastore ()
		IASTORE,
		//store ecx to int/float array element
		IASTOREC,
		//store edx to int/float array element
		IASTORED,

		//set op_stack top to be the const array
		//aconst (const array) 
		ACONST,
		//set eax to be the const array
		ACONSTA,
		//set ebx to be the const array
		ACONSTB,
		//set eax to be the const array
		ACONSTC,
		//set eax to be the const array
		ACONSTD,

		//load local array to the op stack
		//aload (local index)
		ALOAD,
		//load local array to the eax
		ALOADA,
		//load local array to the ebx
		ALOADB,
		//load local array to the ecx
		ALOADC,
		//load local array to the edx
		ALOADD,

		//store op stack to local array
		//astore (local index)
		ASTORE,
		//store eax to local array
		ASTOREA,
		//store ebx to local array
		ASTOREB,
		//store ecx to local array
		ASTOREC,
		//store edx to local array
		ASTORED,

		//load array element to op stack
		//array at eax index at ebx
		//aaload ()
		AALOAD,
		//load array element to eax
		AALOADA,
		//load array element to ebx
		AALOADB,
		//load array element to ecx
		AALOADC,
		//load array element to edx
		AALOADD,

		//store op stack  to array element
		//array at eax index at ebx
		//aastore ()
		AASTORE,
		//store ecx to int/float array element
		AASTOREC,
		//store edx to int/float array element
		AASTORED,

		//int at eax to float
		//i2f()
		I2F,

		//float at eax to int
		//f2i()
		F2I,

		CALL,
		ARET,
		IRET,
		RET,

		ANEW,
		AINIT,
		AGET,
		APUT,
		AFIELDS,

		//goto and jump
		JUMP,
		//jump if eax equal ebx
		JE,
		//jmp if eax not equal ebx
		JNE,
		//jump if eax large than ebx
		JLG,
		//jump if small
		JSM,
		//jump if large or equal
		JLGE,
		//jump if small or equal
		JSME,
		//jump if obj is null
		JNULL,
		//jump if obj not null
		JNNULL,


		//eax = eax + ebx 
		IADD,
		//eax = eax - ebx 
		ISUB,
		//eax = eax * ebx 
		IMUT,
		//eax = eax / ebx 
		IDIV,

		ISHL,
		ISHR,
		IOR,
		IAND,
		IXOR,


		IEQU,
		ILG,
		ISML,
		ILGE,
		ISMLE,

		IREM,
		INEG,
		IXNOT,
		IINC,

		AND,
		OR,
		NOT,

		//static field
		GETSTATIC,
		PUTSTATIC,

		//sync
		MONENTER,
		MONEXIT,

		//push / pop
		//push int/float at eax to stack top
		//push_i()
		PUSH_I,
		//push obj at eax to stack top
		//push_o()
		PUSH_O,
		//push empty array at eax to stack top
		//push_a()
		PUSH_A,

		//push int/float to stack top
		//push_iconst(int i)
		PUSH_ICONST,
		//push obj(NULl) to stack top
		//push_null()
		PUSH_NULL,

		//create obj and push to stack top
		//new_o()
		NEW_O,

		//throw
		ATHROW,
		ACATCH,
		IS_TYPE,

	};

}

#endif