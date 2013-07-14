#pragma once

#include "LANConnection.h"

namespace network
{
	/**
	 * Implementiert die Funktionalit�t zum Ver�ffentlichen eines Servers/Spieles im lokalen Netz.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CLANServer : public CLANConnectionListener
	{
	private:
		CLANConnection *m_pConnection;			///< Kommunikationsthread

		CString m_strDescription;				///< Beschreibung, die ver�ffentlicht werden soll
		UINT m_nTCPPort;						///< zu ver�ffentlichender TCP-Port, an dem der BotE-Server l�uft
		
	public:
		CLANServer();
		virtual ~CLANServer();

		/**
		 * Startet den serverseitigen Kommunikationsthread, der sich um das Ver�ffentlichen eines Spieles
		 * k�mmert.
		 *
		 * @param nServerPort UDP-Port, an dem der Thread l�uft
		 * @param strDescription Beschreibung des Servers/Spieles
		 * @param nTCPPort TCP-Port, an dem der BotE-Server l�uft
		 * @return <code>TRUE</code> gdw. der Thread erfolgreich erzeugt und gestartet werden konnte;
		 * liefert <code>FALSE</code>, wenn der Thread bereits erzeugt wurde
		 */
		BOOL StartPublishing(UINT nServerPort, const CString &strDescription, UINT nTCPPort);

		/// H�lt den serverseitigen Kommunikationsthread an, falls dieser l�uft.
		void StopPublishing();

		// Handler f�r LANConnection-Ereignisse
		virtual void OnMessageReceived(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnMessageSent(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnMessageDiscarded(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnSocketError(int nWSAError, CLANConnection *pConnection);
		virtual void OnConnectionLost(CLANConnection *pConnection);

	};

};

/// globale Instanz
extern network::CLANServer serverPublisher;
