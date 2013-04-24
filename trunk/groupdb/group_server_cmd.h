#ifndef GROUP_SERVER_CMD_H
#define GROUP_SERVER_CMD_H

#pragma once

#include "group_server_type.h"

#pragma pack(push,1)

struct t_group_cmd_header
{
    uint32 magic; //0x20120604  host order	
    uint32 len;    
    uint16 cmd;
    uint64 seq;

};

inline const uint8* buffer_to_struct(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_group_cmd_header& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.magic);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.len);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.cmd);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.seq);

	return rest;	
}

inline uint8* struct_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_group_cmd_header& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.magic);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.len);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.cmd);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.seq);

	return rest;	
}

// client->server
const uint16 k_gcmd_client_keepalive = 10000;
struct t_gcmd_client_keepalive
{
	t_group_cmd_header header;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_client_keepalive& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	return rest;
}


// server->client
const uint16 k_gcmd_server_keepalive = 10001;
struct t_gcmd_server_keepalive
{
	t_group_cmd_header header;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_server_keepalive& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	return rest;
}

// client->server
const uint16 k_gcmd_auth_request = 1001;
struct t_gcmd_auth_request
{
	t_group_cmd_header header;
	uint16 version;		//新增，客户端协议版本	
	uint8  enc_token[16];//16字节，由xmpp返回
	uint16 reserved;
	std::string client_uin;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_auth_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.version);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.enc_token);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.reserved);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.client_uin);

	return rest;
}

// server->client
const uint16 k_gcmd_auth_challenge = 1002;
struct t_gcmd_auth_challenge
{
	t_group_cmd_header header;
	uint32 result; //新增，不支持客户端协议版本时，服务器端返回错误标志，否则返回成功标志
	uint64 challenge;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_auth_challenge& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.challenge);
	return rest;
}

// client->server
const uint16 k_gcmd_challenge_response = 1003;
struct t_gcmd_challenge_response
{
	t_group_cmd_header header;
	uint8 hmac[20]; // hmac_sha(key=challenge,buffer = token);
	//uint16 version; //废弃，提前到t_gcmd_auth_request 中
	t_user_cookie cookie;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_challenge_response& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.hmac);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.cookie);

	return rest;
}

//rc4_key = hmac_sha(key=token,buffer = challenge);

// server->client
const uint16 k_gcmd_auth_response = 1004;
struct t_gcmd_auth_response
{
	t_group_cmd_header header;
	uint32 result;
	std::vector<uint64> static_group_ids;	// 当前该用户所在的静态群
	std::vector<uint64> dynamic_group_ids;	// 当前用户所在的动态群
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_auth_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.static_group_ids);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.dynamic_group_ids);
	return rest;
}

// client->server 开始接收消息
const uint16 k_gcmd_connect_request = 2000;
struct t_gcmd_connect_request
{
    t_group_cmd_header header;
    uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_connect_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->client
const uint16 k_gcmd_connect_response = 2001;
struct t_gcmd_connect_response
{
    t_group_cmd_header header;
    uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_connect_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);
	return rest;
}

// client->server
const uint16 k_gcmd_get_ginfo_request = 2002;
struct t_gcmd_get_ginfo_request
{
    t_group_cmd_header header;
    uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_get_ginfo_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->client
const uint16 k_gcmd_get_ginfo_response = 2003;
struct t_gcmd_get_ginfo_response
{
	t_group_cmd_header header;
	t_ginfo group_info;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_get_ginfo_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_info);
	return rest;
}

// client->server
const uint16 k_gcmd_get_lastmsg_request = 2004;
struct t_gcmd_get_lastmsg_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_get_lastmsg_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->client
const uint16 k_gcmd_get_lastmsg_response = 2005;
struct t_gcmd_get_lastmsg_response
{
	t_group_cmd_header header;
	uint64 group_id;
	std::vector<t_gmessage> messages;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_get_lastmsg_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.messages);	
	return rest;
}

// client->server
const uint16 k_gcmd_get_lastcmd_request = 2006;
struct t_gcmd_get_lastcmd_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_get_lastcmd_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->client (这个包后面就是连续的notify)
const uint16 k_gcmd_get_lastcmd_response = 2007;
struct t_gcmd_get_lastcmd_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 cmd_count;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_get_lastcmd_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.cmd_count);	
	return rest;
}

// client->server
const uint16 k_gcmd_create_request = 2008;
struct t_gcmd_create_request
{
    t_group_cmd_header header;
    uint8 group_type;
    uint8 group_verify;
    uint64 group_flag;
    uint64 group_attr;//新加
    std::string owner_name;//群主在此群的昵称
    std::string group_name; //群名称
    std::string group_intro;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_create_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_type);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_verify);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_flag);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_attr);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.owner_name);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_name);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_intro);

	return rest;
}

// server->client
const uint16 k_gcmd_create_response = 2009;
struct t_gcmd_create_response
{
	t_group_cmd_header header;
	uint32 result;
	t_ginfo group_info;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_create_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_info);	
	return rest;
}

// admin->server
const uint16 k_gcmd_dismiss_request = 2010;
struct t_gcmd_dismiss_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_dismiss_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->admin
const uint16 k_gcmd_dismiss_response = 2011;
struct t_gcmd_dismiss_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_dismiss_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);	
	return rest;
}

// server->client
const uint16 k_gcmd_dismiss_notify = 3000;
struct t_gcmd_dismiss_notify
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_dismiss_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);

	return rest;
}

// client->server
const uint16 k_gcmd_dismiss_confirm = 4000;
struct t_gcmd_dismiss_confirm
{
	t_group_cmd_header header;
	uint64 group_id;	
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_dismiss_confirm& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// client->server
const uint16 k_gcmd_chat_request = 2012;
struct t_gcmd_chat_request
{
    t_group_cmd_header header;
    uint64 group_id;
    uint8 msg_type;
    uint8 msg_flag;
    uint32 msg_seq;
    std::string message;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_chat_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.msg_type);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.msg_flag);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.msg_seq);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.message);

	return rest;
}

// server->client
const uint16 k_gcmd_chat_response = 2013;
struct t_gcmd_chat_ack
{
    t_group_cmd_header header;
    uint64 group_id;
    uint32 msg_seq;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chat_ack& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.msg_seq);	
	return rest;
}

// server->client
const uint16 k_gcmd_chat_notify = 3001;
struct t_gcmd_chat_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	t_gmessage message;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chat_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.message);	
	return rest;
}

// client->server
const uint16 k_gcmd_chgstatus_request = 2014;
struct t_gcmd_chgstatus_request
{
	t_group_cmd_header header;
	uint8 status;
	std::vector<uint64> group_ids;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_chgstatus_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.status);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_ids);

	return rest;
}

// server->client
const uint16 k_gcmd_chgstatus_response = 2015;
struct t_gcmd_chgstatus_response
{
	t_group_cmd_header header;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chgstatus_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}


// server->client
const uint16 k_gcmd_chgstatus_notify = 3002;
struct t_gcmd_chgstatus_notify
{
	t_group_cmd_header header;
	uint8 status;
	t_user_cookie cookie;
	std::string uin;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chgstatus_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.status);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.cookie);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.uin);
	return rest;
}

// client->server 1.name,2sex,3phone,4email,5comment,6msgset
const uint16 k_gcmd_chguinfo_request = 2016;
struct t_gcmd_chguinfo_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::vector<std::pair<uint8,std::string> > infos;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_chguinfo_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.infos);

	return rest;
}

// server->client
const uint16 k_gcmd_chguinfo_response = 2017;
struct t_gcmd_chguinfo_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chguinfo_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}

// server->client
const uint16 k_gcmd_chguinfo_notify = 3003;
struct t_gcmd_chguinfo_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string uin;
	std::string name;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chguinfo_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.uin);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.name);
	return rest;
}

// client->server
const uint16 k_gcmd_getuinfo_request = 2018;
struct t_gcmd_getuinfo_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string uin;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_getuinfo_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.uin);

	return rest;
}

// server->client
const uint16 k_gcmd_getuinfo_response = 2019;
struct t_gcmd_getuinfo_response
{
	t_group_cmd_header header;
	uint64 group_id;
	t_guser_info2 info;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_getuinfo_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.info);
	return rest;
}

// client->server
const uint16 k_gcmd_getuinfos_request = 2020;
struct t_gcmd_getuinfos_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_getuinfos_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->client
const uint16 k_gcmd_getuinfos_response = 2021;
struct t_gcmd_getuinfos_response
{
	t_group_cmd_header header;
	uint64 group_id;
	std::vector<t_guser_info> infos;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_getuinfos_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.infos);
	return rest;
}

// client->server
const uint16 k_gcmd_quit_request = 2022;
struct t_gcmd_quit_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_quit_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->client
const uint16 k_gcmd_quit_response = 2023;
struct t_gcmd_quit_response
{
	t_group_cmd_header header;
	uint64 group_id;	
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_quit_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);
	return rest;
}

// server->client
const uint16 k_gcmd_quit_notify = 3004;
struct t_gcmd_quit_notify
{
	t_group_cmd_header header;
	uint64 group_id;	
	std::string quit_uin;
	std::string quit_name;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_quit_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.quit_uin);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.quit_name);
	return rest;
}

// client->server
const uint16 k_gcmd_setrole_request = 2024;
struct t_gcmd_setrole_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string target_uin;
	uint8 target_role;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_setrole_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.target_uin);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.target_role);

	return rest;
}

// server->client
const uint16 k_gcmd_setrole_response = 2025;
struct t_gcmd_setrole_response
{
	t_group_cmd_header header;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_setrole_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}

//below start

// server->client
const uint16 k_gcmd_setrole_notify = 3005;
struct t_gcmd_setrole_notify
{
	t_group_cmd_header header;// 如果client是user，那么seq为0，否则seq不为0
	uint64 group_id;
	std::string admin_uin;
	std::string target_uin;
	uint8 target_role;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_setrole_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.admin_uin);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.target_uin);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.target_role);
	return rest;
}

// admin->server
const uint16 k_gcmd_setrole_confirm = 4001;
struct t_gcmd_setrole_confirm
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_setrole_confirm& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	
	return rest;
}

// client->server 1group_flag,2group_name,3group_bulletin,4group_intro,5group_verify
const uint16 k_gcmd_chgginfo_request = 2026;
struct t_gcmd_chgginfo_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::vector<std::pair<uint8,std::string> > info;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_chgginfo_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.info);
	
	return rest;
}

// server->client
const uint16 k_gcmd_chgginfo_response = 2027;
struct t_gcmd_chgginfo_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chgginfo_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}

// server->client
const uint16 k_gcmd_chgginfo_notify = 3006;
struct t_gcmd_chgginfo_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	std::vector<std::pair<uint8,std::string> > info;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chgginfo_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.info);

	return rest;
}

// client->server
const uint16 k_gcmd_kick_request = 2028;
struct t_gcmd_kick_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string target_uin;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_kick_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.target_uin);
	
	return rest;
}

// server->client
const uint16 k_gcmd_kick_response = 2029;
struct t_gcmd_kick_response
{
	t_group_cmd_header header;
	uint64 group_id;	
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_kick_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}

// server->client
const uint16 k_gcmd_kick_notify = 3007;
struct t_gcmd_kick_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string admin_uin;
	std::string target_uin;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_kick_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.admin_uin);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.target_uin);
	return rest;
}

// client->server // 静态群的被踢者回应此消息
const uint16 k_gcmd_kick_confirm = 4002;
struct t_gcmd_kick_confirm
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_kick_confirm& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	
	return rest;
}

// client->server
const uint16 k_gcmd_join_request = 2030;
struct t_gcmd_join_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string name;
	std::string reason;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_join_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.name);
	if ( !rest )
		return rest;	

	rest = buffer_to_type(rest, buflen, v.reason);

	return rest;
}

// server->client
const uint16 k_gcmd_join_response = 2031;
struct t_gcmd_join_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_join_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}

// server->client join user 
const uint16 k_gcmd_join_notify = 3008;
struct t_gcmd_join_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	bool allow;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_join_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.allow);

	return rest;
}

//join user client -> server
const uint16 k_gcmd_join_confirm = 4003;
struct t_gcmd_join_confirm
{
	t_group_cmd_header header;
	uint64 group_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_join_confirm& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);

	return rest;
}

// server->admin
const uint16 k_gcmd_join_notify2 = 3009;
struct t_gcmd_join_notify2
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string uin;
	std::string name;
	std::string reason;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_join_notify2& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.uin);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.name);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.reason);
	return rest;
}

// server->client (发给其他用户)
const uint16 k_gcmd_newuser_notify = 3010;
struct t_gcmd_newuser_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string uin;
	std::string name;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_newuser_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.uin);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.name);
	return rest;
}

// admin->server
const uint16 k_gcmd_join_confirm2 = 4004;
struct t_gcmd_join_confirm2
{
	t_group_cmd_header header;
	uint64 group_id;
	bool allow;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_join_confirm2& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.allow);
	
	return rest;
}

// admin->server
const uint16 k_gcmd_invite_request = 2032;
struct t_gcmd_invite_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string invite_uin;
	std::string invite_reason;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_invite_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.invite_uin);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.invite_reason);
	
	return rest;
}

// server->admin
const uint16 k_gcmd_invite_response = 2033;
struct t_gcmd_invite_response
{
	t_group_cmd_header header;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_invite_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);
	return rest;
}

// server->invitee
const uint16 k_gcmd_invite_notify = 3011;
struct t_gcmd_invite_notify
{
	t_group_cmd_header header;
	std::string  invite_uin; 	//谁邀请的我
	std::string invite_name; 	//邀请人的名字
	uint64 group_id;
	uint8  group_type;          //动态群还是静态群
	std::string group_name;
	std::string invite_reason;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_invite_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.invite_uin);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.invite_name);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_type);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_name);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.invite_reason);	
	return rest;
}

// invitee->server
const uint16 k_gcmd_invite_confirm = 4005;
struct t_gcmd_invite_comfirm
{
	t_group_cmd_header header;
	uint64 group_id;
	bool allow;
	std::string nickname;  //在这个群里的昵称
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_invite_comfirm& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.allow);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.nickname);
	
	return rest;
}

// server->admin
const uint16 k_gcmd_invite_notify2 = 3012;
struct t_gcmd_invite_notify2
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string group_name;
	bool allow;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_invite_notify2& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_name);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.allow);

	return rest;
}

// client->server
const uint16 k_gcmd_file_share_request = 2034;
struct t_gcmd_file_share_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_offline_info offline_info;
	t_task_id task_id;
	uint64 total_size;
	uint32 file_count;
	uint32 path_count;
	bool folder;
	std::string task_name;
	std::string task_attr;
	std::string task_desc;
	std::string task_seed;	// t_u2u_recv_task_info
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_file_share_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.offline_info);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.task_id);
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
	
	rest = buffer_to_type(rest, buflen, v.task_seed);
	
	return rest;
}

// server->client
const uint16 k_gcmd_file_share_response = 2035;
struct t_gcmd_file_share_response
{
	t_group_cmd_header header;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_file_share_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}

// server->client
const uint16 k_gcmd_file_share_notify = 3013;
struct t_gcmd_file_share_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	t_gfile_info file_info;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_file_share_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.file_info);

	return rest;
}

//
// client->server
const uint16 k_gcmd_file_finish_request = 2036;
struct t_gcmd_file_finish_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_file_finish_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.task_id);
	
	return rest;
}

// server->client
const uint16 k_gcmd_file_finish_response = 2037;
struct t_gcmd_file_finish_response
{
	t_group_cmd_header header;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_file_finish_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);

	return rest;
}

// client->server
const uint16 k_gcmd_get_files_request = 2038;
struct t_gcmd_get_files_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string filter;	// 一个sql语句，按时间过滤，发布者过滤，文件名过滤，描述过滤
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_get_files_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.filter);
	
	return rest;
}

// server->client
const uint16 k_gcmd_get_files_response = 2039;
struct t_gcmd_get_files_response
{
	t_group_cmd_header header;
	uint64 group_id;
	std::vector<t_gfile_info> files;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_get_files_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.files);
	return rest;
}

// client->server (只有owner或者admin可以修改共享文件的描述)
// 1: task_name; 2: task_desc
const uint16 k_gcmd_chg_file_request = 2040;
struct t_gcmd_chg_file_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
	std::vector<std::pair<uint8,std::string> > gfile_infos;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_chg_file_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.task_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.gfile_infos);
	
	return rest;
}

// server->client
const uint16 k_gcmd_chg_file_response = 2041;
struct t_gcmd_chg_file_response
{
	t_group_cmd_header header;
	uint32 result;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_chg_file_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.result);
	return rest;
}

// client->server
const uint16 k_gcmd_get_file_request = 2042;
struct t_gcmd_get_file_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_get_file_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.task_id);
	
	return rest;
}

// server->client
const uint16 k_gcmd_get_file_response = 2043;
struct t_gcmd_get_file_response
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
	std::string task_seed;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_get_file_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.task_id);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.task_seed);
	return rest;
}

// client->server
const uint16 k_gcmd_get_history_request = 2044;
struct t_gcmd_get_history_request
{
	t_group_cmd_header header;
	uint64 group_id;
	std::string filter;
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_get_history_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.filter);
	
	return rest;
}

// server->client
const uint16 k_gcmd_get_history_response = 2045;
struct t_gcmd_get_history_response
{
	t_group_cmd_header header;
	uint64 group_id;
	std::vector<t_gmessage> messages;
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_get_history_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.group_id);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.messages);
	return rest;
}

// server->client
const uint16 k_gcmd_kickoff_notify = 3014;
struct t_gcmd_kickoff_notify
{
	t_group_cmd_header header;
	uint32 ip;//被哪个IP踢掉
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_kickoff_notify& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.ip);
	return rest;
}

// client->server
const uint16 k_gcmd_get_usergroups_request = 2046;
struct t_gcmd_get_usergroups_request
{
	t_group_cmd_header header;
	std::string uin;	
};

inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_get_usergroups_request& v)
{
	const uint8 *rest = buffer_to_struct(buffer, buflen, v.header);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.uin);
	
	return rest;
}

// server->client
const uint16 k_gcmd_get_usergroups_response = 2047;
struct t_gcmd_get_usergroups_response
{
	t_group_cmd_header header;
	std::string uin;	
	std::vector<uint64> static_group_ids;	// 当前该用户所在的静态群
};

inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_get_usergroups_response& v)
{
	uint8 *rest = struct_to_buffer(buffer, buflen, v.header);
	if ( !rest )
		return rest;

    rest = type_to_buffer(rest, buflen, v.uin);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.static_group_ids);
	return rest;
}

template<typename T>
struct t_gcmd_connect2group
{
	uint32 len; //total length
	std::string uin;
	T body;
};

template<typename T>
struct t_gcmd_group2connect
{
	uint32 len; //total length
	std::vector<std::string> uins;
	T body;
};


template<typename T>
inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_connect2group<T>& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.len);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.uin);
	if ( !rest )
		return rest;

	rest = buffer_to_cmd(rest, buflen, v.body);
	
	return rest;
}

template<typename T>
inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_connect2group<T>& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.len);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.uin);
	if ( !rest )
		return rest;

	rest = cmd_to_buffer(rest, buflen, v.body);
	
	return rest;
}

template<typename T>
inline const uint8* buffer_to_cmd(IN_P const uint8* buffer, INOUT_P uint32& buflen, OUT_P t_gcmd_group2connect<T>& v)
{
	const uint8 *rest = buffer_to_type(buffer, buflen, v.len);
	if ( !rest )
		return rest;

	rest = buffer_to_type(rest, buflen, v.uins);
	if ( !rest )
		return rest;

	rest = buffer_to_cmd(rest, buflen, v.body);
	
	return rest;
}

template<typename T>
inline uint8* cmd_to_buffer(OUT_P uint8* buffer, INOUT_P uint32& buflen, IN_P t_gcmd_group2connect<T>& v)
{
	uint8 *rest = type_to_buffer(buffer, buflen, v.len);
	if ( !rest )
		return rest;

	rest = type_to_buffer(rest, buflen, v.uins);
	if ( !rest )
		return rest;

	rest = cmd_to_buffer(rest, buflen, v.body);
	
	return rest;
}


#pragma pack(pop)

#endif

