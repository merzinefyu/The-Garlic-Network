/**
*	namespace.cpp - Модуль отвечающий за работу
*	функций пространства имен tgnrouter.
*
*	@mrrva - 2019
*/
#include "../include/router.hpp"
/**
*	Используемые пространства имен и объекты.
*/
using namespace std;
/**
*	tgnrouter::client - Функция обработки сообщений
*	от клиентов сети.
*
*	@msg - Сообщение от клиента сети.
*	@skddr - Структура sockaddr_in.
*/
unsigned char *tgnrouter::client(tgnmsg msg,
		struct sockaddr_in skddr)
{
	enum tgn_htype type = msg.header_type();
	unsigned char *resp = nullptr;

	if (type == INDEFINITE_MESSAGE || type >= 0x10
		|| type == U_REQUEST_DOS) {
		resp = msg_tmp<true>(U_RESPONSE_DOS);
		return resp;
	}

	switch(type) {
	case U_REQUEST_NODES:
		resp = router.req_nodes(msg, skddr, true);
		break;

	case U_REQUEST_GARLIC:
		resp = router.client_garlic(msg, skddr);
		break;

	case U_RESPONSE_GARLIC:
		resp = router.c_status_garlic(msg, skddr);
		return resp;

	default:
		resp = nullptr;
		break;
	}

	if (resp == nullptr)
		resp = msg_tmp<true>(U_RESPONSE_DOS);

	return resp;
}
/**
*	tgnrouter::node - Функция обработки сообщений
*	от нод сети.
*
*	@msg - Сообщение от ноды сети.
*	@skddr - Структура sockaddr_in.
*/
unsigned char *tgnrouter::node(tgnmsg msg,
	struct sockaddr_in skddr)
{
	enum tgn_htype type = msg.header_type();
	unsigned char *resp = nullptr;

	if (type == INDEFINITE_MESSAGE || type <= 0x10
		|| type == S_REQUEST_DOS) {
		resp = msg_tmp<true>(S_RESPONSE_DOS);
		return resp;
	}

	tgnstorage::nodes.ping(skddr);

	switch (type) {
	case S_REQUEST_NODES:
		resp = router.req_nodes(msg, skddr, false);
		break;

	case S_RESPONSE_NODES:
		resp = router.rsp_nodes(msg, skddr);
		return resp;

	case S_REQUEST_FIND:
		resp = router.req_find(msg, skddr);
		break;

	case S_RESPONSE_FIND:
		resp = router.rsp_find(msg, skddr);
		return resp;

	case S_REQUEST_CLIENTS:
		resp = router.req_neighbors(msg, skddr);
		break;

	case S_RESPONSE_CLIENTS:
		resp = router.rsp_neighbors(msg, skddr);
		return resp;

	case S_REQUEST_GARLIC:
		resp = router.node_garlic(msg, skddr);
		break;

	case S_RESPONSE_GARLIC:
		resp = router.n_status_garlic(msg, skddr);
		return resp;

	default:
		resp = nullptr;
		break;
	}

	if (resp == nullptr)
		resp = msg_tmp<true>(U_RESPONSE_DOS);

	return resp;
}