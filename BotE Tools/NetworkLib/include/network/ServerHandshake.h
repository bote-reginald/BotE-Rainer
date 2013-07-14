#pragma once
#include "thread.h"

namespace network
{
	/**
	 * Thread, der serverseitig den Handshake mit dem Client ausf�hrt. Informiert die
	 * <code>CBotEServer</code>-Instanz per <code>OnNewClient()</code> �ber die
	 * Erfolgreiche Durchf�hrung eines Handshakes.
	 *
	 * <code>CServerHandshake</code>-Objekte zerst�ren sich nach Beendigung des Threads
	 * von selbst.
	 *
	 * Handshake (three-way):
	 *
	 * 1. Client sendet		HANDSHAKE_CLIENTID + "\0" + <UINT Versionsnummer> + "\0"
	 * 2. Server sendet		HANDSHAKE_SERVERID + "\0" + <UINT Versionsnummer> + "\0"
	 * 3. Client sendet		"OK\0" + <UINT L�nge des Benutzernamens> + <BYTE[] Benutzername>
	 *
	 * Der Partner muss die Nachricht innerhalb von HANDSHAKE_TIMEOUT Sekunden erhalten,
	 * sonst wird die Verbindung getrennt.
	 *
	 * Geben Server und Client verschiedene Versionsnummern an, wird die jeweils kleinere
	 * Versionsnummer verwendet (unter der Annahme, dass zuk�nftige Versionen abw�rtskompatibel
	 * sind).
	 *
	 * @author CBot
	 * @version 0.0.5
	 */
	class CServerHandshake : public CThread
	{
		DECLARE_DYNAMIC(CServerHandshake)

	private:
		SOCKET m_hSocket;
		UINT m_nServerVersion;

	public:
		CServerHandshake(SOCKET hSocket, UINT nServerVersion);
		virtual ~CServerHandshake();

		/// @return <code>0</code> bei Erfolg, sonst <code>1</code>
		virtual void Run();

	private:
		/// Wartet, bis die angegebene Anzahl Bytes zur Verf�gung steht oder die gegebene
		/// Deadline abl�uft.
		static BOOL RequestData(CAsyncSocket &socket, UINT nCount, clock_t clkDeadline);

	};

};
