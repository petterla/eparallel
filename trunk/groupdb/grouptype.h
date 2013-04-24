#pragma once

struct t_v4_addr
{
	t_v4_addr(uint32 ip = 0,uint16 port = 0)
		:ip_(ip),port_(port)
	{
	}
	bool operator == (t_v4_addr const& v) const
	{
		return (ip_ == v.ip_ && port_ == v.port_);
	}
	bool operator != (t_v4_addr const& v) const
	{
		return (ip_ != v.ip_ || port_ != v.port_);
	}
	bool operator < (t_v4_addr const& v) const
	{
		if(ip_ < v.ip_) return true;
		else if(ip_ > v.ip_) return false;

		if(port_ < v.port_) return true;
		else if(port_ > v.port_) return false;

		return false;
	}
	bool operator > (t_v4_addr const& v) const
	{
		if(ip_ > v.ip_) return true;
		else if(ip_ < v.ip_) return false;

		if(port_ > v.port_) return true;
		else if(port_ < v.port_) return false;

		return false;
	}
	uint32 ip_;
	uint16 port_;
};

struct t_user_cookie
{
	t_user_cookie(uint64 data1=0,uint64 data2=0):data1_(data1),data2_(data2){}
	t_user_cookie(t_user_cookie const& s):data1_(s.data1_),data2_(s.data2_){}
	t_user_cookie(uint8 const* data) // Data must at least 16 bytes
	{
		data1_ = *(uint64*)(data);
		data2_ = *(uint64*)(data+8);
	}
	bool zero() const{return (!data1_) && (!data2_);}
	bool operator<(t_user_cookie const& s) const
	{
		if(data1_ < s.data1_) return true;
		else if(data1_ > s.data1_) return false;
		
		if(data2_ < s.data2_) return true;
		else return false;
	}
	bool operator==(t_user_cookie const& s) const
	{
		return (data1_ == s.data1_) && (data2_ == s.data2_);
	}
	bool operator!=(t_user_cookie const& s) const
	{
		return (data1_ != s.data1_) || (data2_ != s.data2_);
	}
	uint64 data1_;
	uint64 data2_;
};

struct t_task_id
{
	t_task_id(uint64 data1=0,uint64 data2=0):data1_(data1),data2_(data2){}
	t_task_id(t_task_id const& s):data1_(s.data1_),data2_(s.data2_){}
	t_task_id(uint8 const* data) // Data must at least 16 bytes
	{
		data1_ = *(uint64*)(data);
		data2_ = *(uint64*)(data+8);
	}
	bool zero() const{return (!data1_) && (!data2_);}
	bool operator<(t_task_id const& s) const
	{
		if(data1_ < s.data1_) return true;
		else if(data1_ > s.data1_) return false;
		
		if(data2_ < s.data2_) return true;
		else return false;
	}
	bool operator==(t_task_id const& s) const
	{
		return (data1_ == s.data1_) && (data2_ == s.data2_);
	}
	bool operator!=(t_task_id const& s) const
	{
		return (data1_ != s.data1_) || (data2_ != s.data2_);
	}
	uint64 data1_;
	uint64 data2_;
};

struct t_offline_info
{
	t_v4_addr tcp_addr() const{return t_v4_addr(ip,tcp_port);}
	uint32 ip;
	uint16 tcp_port;
	uint8 token[16];
	uint8 enc_token[16];
};


struct t_group_token
{
	t_group_token(uint8 const* data)
	{
		if(data) memcpy(data_,data,16);
		else memset(data_,0,16);
	}
	uint8 data_[16];
};

const uint8 k_ginfo_verify_open = 0;
const uint8 k_ginfo_verify_check = 1;
const uint8 k_ginfo_verify_close = 2;

const uint8 k_ginfo_type_dymatic = 0;
const uint8 k_ginfo_type_static_1 = 1;	// 静态群的3个级别,可能有不同的权限和最大用户数
const uint8 k_ginfo_type_static_2 = 2;
const uint8 k_ginfo_type_static_3 = 3;

struct t_ginfo
{
    uint64 group_id;
	t_uin owner_uin;
    wstring owner_name;
    __time64_t create_time;
    uint8 verify;
    uint8 type;
    wstring intro;
    wstring bulletin; // group bulletin
    wstring name;	// group name
    uint64 flag;	// group flag, useless now    
    uint64 group_attr; //群属性标志，值为位标志值(1,2,4,8...)

};

struct t_gmessage
{
	uint8 type;	// useless now
	uint8 flag;	// 用于更改上一条消息
	uint32 seq;
	__time64_t time;
	t_uin sender_uin;
	wstring message;
};

struct t_guser_info
{
	uint8 status;
	uint8 role;
	uint8 sex;
	wstring name;
	t_uin uin;
	t_user_cookie cookie;
	uint64 attrs; //位值(0x01,0x02,0x04....),0x01表示是否允许别人察看自己的群
};

struct t_guser_info2
{
	uint8 status;
	uint8 role;
	uint8 sex;
	wstring name;
	t_uin uin;
	wstring phone;
	wstring email;
	wstring comment;
};

struct t_gfile_info
{
	t_task_id task_id;
	__time64_t task_time;
	t_uin owner_uin;
	uint64 total_size;
	uint32 file_count;
	uint32 path_count;
	bool folder;
	wstring task_name;
	wstring task_attr;
	wstring task_desc;
	bool finished;
};