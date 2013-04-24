#ifndef GROUP_SERVER_TYPE_H
#define GROUP_SERVER_TYPE_H

#ifndef _WIN32
#include <stdint.h>
#endif
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <time.h>

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define IN_P
#define OUT_P
#define INOUT_P

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
	uint32 ip;
	uint16 tcp_port;
	uint8 token[16];
	uint8 enc_token[16];
};

struct t_ginfo
{
    uint64 group_id;
	std::string owner_uin;
    std::string owner_name;
    time_t create_time;
    uint8 verify;
    uint8 type;
    std::string intro;
    std::string bulletin; // group bulletin
    std::string name;	// group name
    uint64 flag;	// group flag, useless now    
    uint64 group_attr; //群属性标志，值为位标志值(1,2,4,8...)

};

struct t_gmessage
{
	uint8 type;	// useless now
	uint8 flag;	// 用于更改上一条消息
	uint32 seq;
	time_t time;
	std::string sender_uin;
	std::string message;
};

struct t_guser_info
{
	uint8 status;
	uint8 role;
	uint8 sex;
	std::string name;
	std::string uin;
	t_user_cookie cookie;
};

struct t_guser_info2
{
	uint8 status;
	uint8 role;
	uint8 sex;
	std::string name;
	std::string uin;
	std::string phone;
	std::string email;
	std::string comment;
	uint64 attrs; //位值(0x01,0x02,0x04....),0x01表示是否允许别人察看自己的群
	uint8 msgset;
	
};

struct t_gfile_info
{
	t_task_id task_id;
	time_t task_time;
	std::string owner_uin;
	uint64 total_size;
	uint32 file_count;
	uint32 path_count;
	bool folder;
	std::string task_name;
	std::string task_attr;
	std::string task_desc;
	bool finished;
};

const uint8 k_ginfo_verify_open = 0;
const uint8 k_ginfo_verify_check = 1;
const uint8 k_ginfo_verify_close = 2;

const uint8 k_ginfo_type_dymatic = 0;
const uint8 k_ginfo_type_static_1 = 1;	// 静态群的3个级别,可能有不同的权限和最大用户数
const uint8 k_ginfo_type_static_2 = 2;
const uint8 k_ginfo_type_static_3 = 3;


//support uint8 uint16 uint32 uint64 array
template<class T>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P T& v)
{
	size_t outlen = sizeof(T);
	if ( buflen < outlen )
		return NULL;

	memcpy(&v, buffer, outlen);
	buflen -= outlen;
	return buffer + outlen;
}

//support uint8 uint16 uint32 uint64 array
template<class T>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P T& v)
{
	size_t outlen = sizeof(T);
	if ( buflen < outlen )
		return NULL;

	memcpy(buffer, &v, outlen);
	buflen -= outlen;
	return buffer + outlen;
}

template<class T>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P std::vector<T>& vec)
{
	uint32 vector_size=0;
	const uint8 *rest = buffer_to_type(buffer, buflen, vector_size);
	if ( !rest )
		return rest;

	typename std::vector<T>::iterator it = vec.begin();
	
	for(uint32 i=0; i<vector_size; ++i)
	{
		T v;
		rest = buffer_to_type(rest, buflen, v);
		if ( !rest )
			return rest;
		it = vec.insert(it, v);
	}

	return rest;
}

template<class T>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P std::vector<T>& vec)
{
	uint32 vector_size=vec.size();
	uint8 *rest = type_to_buffer(buffer, buflen, vector_size);
	if ( !rest )
		return rest;

	for(uint32 i=0; i<vector_size; ++i)
	{
		rest = type_to_buffer(rest, buflen, vec[i]);
		if ( !rest )
			return rest;
	}

	return rest;
}

template<class T1, class T2>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P std::pair<T1,T2>& pair)
{
	T1 v1;
	const uint8 *rest = buffer_to_type(buffer, buflen, v1);
	if ( !rest )
		return rest;

	T2 v2;
	rest = buffer_to_type(rest, buflen, v2);
	if ( !rest )
		return rest;

	pair.first = v1;
	pair.second = v2;

	return rest;
}

template<class T1, class T2>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P std::pair<T1,T2>& pair)
{
	uint8 *rest = type_to_buffer(buffer, buflen, pair.first);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, pair.second);

	return rest;
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P std::string& v)
{
	uint32 vlen=0,offset=0;
	memcpy(&vlen, buffer+offset, 4);
	offset += 4;
	if (vlen >= buflen-offset)
		return NULL;
	
	v.assign(reinterpret_cast<const char*>(buffer+offset), vlen);
	offset += vlen;
	buflen -= offset;
	return buffer + offset;
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P std::string& v)
{
	uint32 str_size=v.size();
	uint8 *rest = type_to_buffer(buffer, buflen, str_size);
	if ( !rest )
		return rest;

	if ( buflen < str_size )
		return NULL;
		
	memcpy(rest, v.c_str(), str_size);
	buflen -= str_size;

	return rest+str_size;
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_v4_addr& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.ip_);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.port_);

	return rest;	
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_v4_addr& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.ip_);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.port_);

	return rest;	
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_user_cookie& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.data1_);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.data2_);

	return rest;	
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_user_cookie& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.data1_);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.data2_);

	return rest;	
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_task_id& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.data1_);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.data2_);

	return rest;	
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_task_id& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.data1_);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.data2_);

	return rest;	
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_offline_info& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.ip);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.tcp_port);
	if ( !rest )
		return rest;

	
	rest = buffer_to_type(rest, buflen, v.token);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.enc_token);
	
	return rest;	
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_offline_info& v)
{
    uint8 *rest = type_to_buffer(buffer, buflen, v.ip);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.tcp_port);
	if ( !rest )
		return rest;

	
	rest = type_to_buffer(rest, buflen, v.token);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.enc_token);
	
	return rest;	
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_ginfo& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.owner_uin);
	if ( !rest )
		return rest;
	
	rest = buffer_to_type(rest, buflen, v.owner_name);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.create_time);
	if ( !rest )
		return rest;
	
	rest = buffer_to_type(rest, buflen, v.verify);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.type);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.intro);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.bulletin);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.name);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.flag);
	if ( !rest )
		return rest;	
	
	rest = buffer_to_type(rest, buflen, v.group_attr);	
	return rest;	
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_ginfo& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.owner_uin);
	if ( !rest )
		return rest;
	
	rest = type_to_buffer(rest, buflen, v.owner_name);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.create_time);
	if ( !rest )
		return rest;
	
	rest = type_to_buffer(rest, buflen, v.verify);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.type);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.intro);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.bulletin);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.name);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.flag);
	if ( !rest )
		return rest;	
	
	rest = type_to_buffer(rest, buflen, v.group_attr);	
	return rest;	
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gmessage& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.type);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.flag);
	if ( !rest )
		return rest;
	
	rest = buffer_to_type(rest, buflen, v.seq);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.time);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.sender_uin);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.message);	
	return rest;
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gmessage& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.type);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.flag);
	if ( !rest )
		return rest;
	
	rest = type_to_buffer(rest, buflen, v.seq);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.time);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.sender_uin);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.message);	
	return rest;
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_guser_info& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.status);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.role);
	if ( !rest )
		return rest;
	
	rest = buffer_to_type(rest, buflen, v.sex);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.name);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.uin);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.cookie);	
	return rest;
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_guser_info& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.status);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.role);
	if ( !rest )
		return rest;
	
	rest = type_to_buffer(rest, buflen, v.sex);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.name);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.uin);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.cookie);	
	return rest;
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_guser_info2& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.status);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.role);
	if ( !rest )
		return rest;
	
	rest = buffer_to_type(rest, buflen, v.sex);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.name);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.uin);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.phone);
	if ( !rest )
		return rest;	
	
	rest = buffer_to_type(rest, buflen, v.email);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.comment);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.attrs);
	if ( !rest )
		return rest;	
	

	rest = buffer_to_type(rest, buflen, v.msgset);	
	return rest;
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_guser_info2& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.status);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.role);
	if ( !rest )
		return rest;
	
	rest = type_to_buffer(rest, buflen, v.sex);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.name);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.uin);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.phone);
	if ( !rest )
		return rest;	
	
	rest = type_to_buffer(rest, buflen, v.email);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.comment);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.attrs);
	if ( !rest )
		return rest;	
	
	rest = type_to_buffer(rest, buflen, v.msgset);	
	return rest;
}

template<>
inline const uint8* buffer_to_type(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gfile_info& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.task_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.task_time);
	if ( !rest )
		return rest;
	
	rest = buffer_to_type(rest, buflen, v.owner_uin);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.total_size);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.file_count);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.path_count);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.folder);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.task_name);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.task_attr);
	if ( !rest )
		return rest;	
	
	rest = buffer_to_type(rest, buflen, v.task_desc);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.finished);	
	return rest;
}

template<>
inline uint8* type_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gfile_info& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.task_id);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.task_time);
	if ( !rest )
		return rest;
	
	rest = type_to_buffer(rest, buflen, v.owner_uin);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.total_size);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.file_count);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.path_count);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.folder);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.task_name);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.task_attr);
	if ( !rest )
		return rest;	
	
	rest = type_to_buffer(rest, buflen, v.task_desc);
	if ( !rest )
		return rest;	

	rest = type_to_buffer(rest, buflen, v.finished);	
	return rest;
}

#endif

