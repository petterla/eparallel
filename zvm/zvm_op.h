#ifndef ZVM_OP_H
#define ZVM_OP_H

#include "zvm_type.h"


namespace zvm{

	/*****************************
	*           int op           *
	*****************************/
	struct ab{};
	struct ac{};
	struct ad{};
	struct ba{};
	struct bc{};
	struct bd{};
	struct ca{};
	struct cb{};
	struct cd{};
	struct da{};
	struct db{};
	struct dc{};
	struct aopstack{};
	struct bopstack{};
	struct copstack{};
	struct dopstack{};
	struct opstacka{};
	struct opstackb{};
	struct opstackc{};
	struct opstackd{};
	struct iconst{
		s64	m_i;
	};
	struct iconsta{
		s64	m_i;
	};
	struct iconstb{
		s64	m_i;
	};
	struct iconstc{
		s64	m_i;
	};
	struct iconstd{
		s64	m_i;
	};
	struct iload{
		u32 m_idx;
	};
	struct iloada{
		u32 m_idx;
	};
	struct iloadb{
		u32 m_idx;
	};
	struct iloadc{
		u32 m_idx;
	};
	struct iloadd{
		u32 m_idx;
	};
	struct istore{
		u32 m_idx;
	};
	struct istorea{
		u32 m_idx;
	};
	struct istoreb{
		u32 m_idx;
	};
	struct istorec{
		u32 m_idx;
	};
	struct istored{
		u32 m_idx;
	};
	struct iadd{};
	struct isub{};
	struct imut{};
	struct idiv{};
	struct irem{};
	struct ishl{};
	struct ishr{};
	struct ineg{};
	struct ixnot{};
	struct iinc{};
	struct not{};
	struct and{};
	struct or{};
	struct monenter{};
	struct monexit{};
	struct aload{
		u32 m_idx;
	};
	struct aloada{
		u32 m_idx;
	};
	struct aloadb{
		u32 m_idx;
	};
	struct aloadc{
		u32 m_idx;
	};
	struct aloadd{
		u32 m_idx;
	};
	struct push_i{};
	struct push_o{};
	struct push_a{};
	struct push_iconst{
		s64 m_i;
	};
	struct push_null{};
	struct athrow{};
	struct acatch{};
	struct jump{
		u32 m_pc;
	};
	struct je{
		u32 m_pc;
	};
	struct jne{
		u32 m_pc;
	};
	struct jlg{
		u32 m_pc;
	};
	struct jsm{
		u32 m_pc;
	};
	struct jlge{
		u32 m_pc;
	};
	struct jsme{
		u32 m_pc;
	};
	struct jnull{
		u32 m_pc;
	};
	struct jnnull{
		u32 m_pc;
	};

	//when return a obj,the obj load to op stack first
	struct aret{};
	struct iret{};
	struct fret{};
	struct ret{};

	struct anew{
		u32 m_idx;
	};
	struct ainit{};
	//struct 

	struct operation{
		u32 m_op_code;
		union{
			struct iconst  m_iconst;
			struct iconsta m_iconsta;
			struct iconstb m_iconstb;
			struct iconstc m_iconstc;
			struct iconstd m_iconstd;
			struct iload   m_iload;
			struct iloada  m_iloada;
			struct iloadb  m_iloadb;
			struct iloadc  m_iloadc;
			struct iloadd  m_iloadd;
			struct istore  m_istore;
			struct istorea m_istorea;
			struct istoreb m_istoreb;
			struct istorec m_istorec;
			struct istored m_istored;
			struct iadd   m_iadd;
			struct isub   m_isub;
			struct imut   m_imut;
			struct idiv   m_idiv;
			struct irem   m_irem;
			struct ishl   m_ishl;
			struct ishr   m_ishr;
			struct ineg   m_ineg;
			struct ixnot  m_ixnot;
			struct iinc   m_iinc;
			struct not    m_not;
			struct and    m_and;
			struct or     m_or;
			struct monenter m_monenter;
			struct monexit  m_monexit;
			struct aload    m_aload;
			struct aloada   m_aloada;
			struct aloadb   m_aloadb;
			struct aloadc   m_aloadc;
			struct aloadd   m_aloadd;
			struct push_i   m_push_i;
			struct push_o   m_push_o;
			struct push_a   m_push_a;
			struct athrow   m_athrow;
			struct acatch   m_acatch;
			struct jump     m_jump;
			struct je       m_je;
			struct jne      m_jne;
			struct jlg      m_jlg;
			struct jlge     m_jlge;
			struct jsm      m_jsm;
			struct jsme     m_jsme;
			struct jnull    m_jnull;
			struct jnnull   m_jnnull;
			struct ret      m_ret;
			struct iret     m_iret;
			struct aret     m_aret;
			struct push_iconst    m_push_iconst;
			struct push_null      m_push_null;
			struct ab m_ab;
			struct ac m_ac;
			struct ad m_ad;
			struct ba m_ba;
			struct bc m_bc;
			struct bd m_bd;
			struct ca m_ca;
			struct cb m_cb;
			struct cd m_cd;
			struct da m_da;
			struct db m_db;
			struct dc m_dc;
			struct aopstack m_aopstack;
			struct bopstack m_bopstack;
			struct copstack m_copstack;
			struct dopstack m_dopstack;
			struct opstacka m_opstacka;
			struct opstackb m_opstackb;
			struct opstackc m_opstackc;
			struct opstackd m_opstackd;
			struct anew  m_anew;
			struct ainit m_ainit;
			//struct 
		} m_op;

	};



}

#endif/*ZVM_OP_H*/