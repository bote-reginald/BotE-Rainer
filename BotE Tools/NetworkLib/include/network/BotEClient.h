#pragma once

#include "Connection.h"
#include "ChosenRaces.h"

#include "BeginGame.h"
#include "NextRound.h"
#include "ChatMsg.h"

#include "Reference.h"

namespace network
{
	/**
	 * Zust�nde f�r <code>CBotEClient</code>.
	 */
	typedef enum CLT_STATE
	{
		CLT_STATE_DISCONNECTED,			///< Client ist nicht verbunden
		CLT_STATE_PREPAREGAME,			///< Client ist verbunden, die V�lker werden zugeordnet
		CLT_STATE_PLAYING,				///< das Spiel l�uft, der Spieler spielt
		CLT_STATE_WAITFORDATA,			///< das Spiel l�uft, der Client wartet auf Daten der n�chsten Runde oder Kampfdaten vom Server
	}
	CLT_STATE;

	/**
	 * Stellt Handler f�r Client-Ereignisse zur Verf�gung.
	 *
	 * @author CBot
	 * @version 0.0.3
	 */
	class CClientListener
	{
	public:
		/// Die Verbindung zum Server wurde durch Fehler beendet.
		virtual void OnClientDisconnected() =0;

		/// Es ist ein Socket-Fehler aufgetreten.
		virtual void OnClientSocketError(int nError) =0;

		/// Die Zuordnung der V�lker zu Clients (auch das Volk des lokalen Clients) hat sich ge�ndert.
		/// Im Handler und dar�ber hinaus kann mit <code>CBotEClient::IsPlayer()</code>,
		/// <code>CBotEClient::IsPlayedByClient()</code> und <code>CBotEClient::IsPlayedByServer()</code>
		/// die aktuelle Zuordnung ermittelt werden.
		virtual void OnClientChangedRaces() =0;

		/// Das Spiel wurde vom Server gestartet. Die Client-Oberfl�che sollte sich in den Rundenende-Zustand
		/// versetzen, bis das <code>OnNextRound()</code>-Ereignis gemeldet wird, mit dem die erste (oder bei
		/// geladenen Spielst�nden eine sp�tere) Runde beginnt. Die vom Server einmalig gesendeten Daten
		/// k�nnen per <code>CBeginGame::DeserializeToDoc()</code> erhalten werden. Der Zeiger auf das
		/// Nachrichtenobjekt ist tempor�r und darf nach Ende des Handlers nicht mehr verwendet werden.
		virtual void OnBeginGame(CBeginGame *pMsg) =0;

		/// Der Server hat die angegebene <code>CNextRound</code>-Nachricht gesendet, die die Spieldaten der
		/// n�chsten Runde enth�lt und diese Runde beim Client startet. Die gesendeten Daten k�nnen per
		/// <code>CNextRound::DeserializeToDoc()</code> erhalten werden. Im Handler und dar�ber hinaus kann
		/// mit <code>CBotEClient::IsPlayer()</code>, <code>CBotEClient::IsPlayedByClient()</code> und
		/// <code>CBotEClient::IsPlayedByServer()</code> ermittelt werden, welche V�lker mitspielen und ob
		/// sie von einem Client oder dem Server gespielt werden. Der Zeiger auf das Nachrichtenobjekt ist
		/// tempor�r und darf nach Ende des Handlers nicht mehr verwendet werden.
		virtual void OnNextRound(CNextRound *pMsg) =0;

		/// Es ist eine Chatnachricht eingetroffen. Der Zeiger auf das Nachrichtenobjekt ist tempor�r und darf
		/// nach Ende des Handlers nicht mehr verwendet werden.
		virtual void OnChatMsg(CChatMsg *pMsg) =0;

	};

	typedef CList<CClientListener *> CLIENTLISTENER_LIST;

	/**
	 * Kapselt die komplette Client-Funktionalit�t.
	 *
	 * @author CBot
	 * @version 0.0.3
	 */
	class CBotEClient : public CConnectionListener
	{
	private:
		CLT_STATE m_nState;							///< Zustand des Clients
		CLIENTLISTENER_LIST m_lListeners;			///< Liste der Client-Listeners.
		CConnection *m_pConnection;					///< Verbindung zum Server, <code>NULL</code> wenn nicht verbunden

		RACE m_nClientRace;							///< diesem Client zugeordnetes Volk, zu Beginn <code>RACE_NONE</code>
		CString m_strClientUserName;				///< Benutzername des Clients
		
		/// Enth�lt, nachdem eine <code>CChosenRaces</code>- oder <code>CNextRound</code>-Nachricht
		/// eingetroffen ist, die Information, welche V�lker am Spiel teilnehmen und welche davon von
		/// einem Client oder vom Server gespielt werden. Der Eintrag mit dem Index <code>i</code> ist dem Volk
		/// <code>RACE_FIRST + i</code> zugeordnet.
		PLAYER m_pPlayers[RACE_COUNT];
		
		/// Enth�lt die Benutzernamen der von Clients gespielten V�lker, <code>""</code> f�r alle anderen V�lker.
		/// Die Benutzernamen werden durch die <code>CChosenRaces</code>-Nachricht gesetzt. Verl�sst ein Client
		/// das Spiel, wird der zugeh�rige Benutzername mit dem Eintreffen der n�chsten
		/// <code>CNextRound</code>-Nachricht auf <code>""</code> gesetzt.
		CString m_pstrUserNames[RACE_COUNT];

	public:
		CBotEClient();
		virtual ~CBotEClient();

		void AddClientListener(CClientListener *pListener);
		void RemoveClientListener(CClientListener *pListener);

		/// Baut eine Verbindung mit dem BotE-Server an der angegebenen Adresse auf.
		/// @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE<code>
		BOOL Connect(DWORD dwAddr, int nPort, const CString &strUserName);

		/// Beendet die Verbindung zum Server.
		void Disconnect();

		/// Sendet dem Server eine <code>CEndOfRound</code>-Nachricht mit den Daten des angegebenen
		/// Client-Dokuments und beendet die aktuelle Runde.
		void EndOfRound(CPeerData *pDoc);

		/// Sendet eine Chat-Nachricht an alle verbundenen Clients.
		void Chat(CString message);
		/// Sendet eine Chat-Nachricht an die Clients, denen das Volk <code>RACE_FIRST + i</code>
		/// zugeordnet ist, wenn <code>bIsReceiver[i] == TRUE</code> gilt
		void Chat(CString message, BOOL bIsReceiver[RACE_COUNT]);

		// CConnectionListener-Interface
		virtual void OnPacketReceived(const CReference<CPacket> &packet, CConnection *pConnection);
		virtual void OnPacketSent(const CReference<CPacket> &packet, CConnection *pConnection);
		virtual void OnSocketError(int nWSAError, CConnection *pConnection);
		virtual void OnPacketDiscarded(const CReference<CPacket> &packet, CConnection *pConnection);
		virtual void OnConnectionLost(CConnection *pConnection);

		/// Liefert die IP-Adresse und den Port des Servers, zu dem der Client verbunden ist
		void GetRemoteAddr(DWORD &dwIP, UINT &nPort);

		/// Sendet eine <code>CRequestRace</code>-Nachricht mit der Forderung nach dem
		/// angegebenen Volk an den Server.
		void RequestRace(RACE nRace);

		/// @return das dem Client momentan zugeordnete Volk; liefert w�hrend der Spielphase
		/// (<code>IsPlaying() == TRUE</code>) das Volk, das diesem Client tats�chlich zugeordnet wurde
		RACE GetClientRace() const {return m_nClientRace;}

		/// @return <code>TRUE</code> gdw. das angegebene Volk am Spiel teilnimmt; bezieht sich in der
		/// Volkzuordnungsphase auf den aktuellen Verhandlungsstand, in der Spielphase auf den aktuellen
		/// Rundenstand; wird mit jeder <code>CChosenRaces</code>- oder <code>CNextRound</code>-Nachricht
		/// aktualisiert und steht zum Aufruf durch die Handler <code>CClientListener::OnClientChangedRaces()</code>
		/// und <code>CClientListener::OnNextRound()</code> zur Verf�gung
		BOOL IsPlayer(RACE nRace) const;

		/// @return <code>TRUE</code>, wenn das angegebene Volk von einem Client gespielt wird;
		/// <code>FALSE</code>, wenn es vom Server gespielt wird oder nicht mitspielt; wird mit jeder
		/// <code>CChosenRaces</code>- oder <code>CNextRound</code>-Nachricht
		/// aktualisiert und steht zum Aufruf durch die Handler <code>CClientListener::OnClientChangedRaces()</code>
		/// und <code>CClientListener::OnNextRound()</code> zur Verf�gung
		BOOL IsPlayedByClient(RACE nRace) const;

		/// @return <code>TRUE</code>, wenn das angegebene Volk vom Server gespielt wird;
		/// <code>FALSE</code>, wenn es von einem Client gespielt wird oder nicht mitspielt; wird mit jeder
		/// <code>CChosenRaces</code>- oder <code>CNextRound</code>-Nachricht
		/// aktualisiert und steht zum Aufruf durch die Handler <code>CClientListener::OnClientChangedRaces()</code>
		/// und <code>CClientListener::OnNextRound()</code> zur Verf�gung
		BOOL IsPlayedByServer(RACE nRace) const;

		/// @return <code>TRUE</code> gdw. dieser Client zu einem Server verbunden ist
		BOOL IsConnected() const {return m_nState != CLT_STATE_DISCONNECTED;}

		/// @return <code>TRUE</code> gdw. sich dieser Client in der Spielphase befindet
		/// (Client spielt gerade oder wartet auf die Daten f�r die n�chste Runde)
		BOOL IsPlaying() const {return m_nState == CLT_STATE_PLAYING || m_nState == CLT_STATE_WAITFORDATA;}

		/// @return Benutzername des dem angegebenen Volk zugeordneten Clients; <code>""</code>, wenn das Volk
		/// vom Server gespielt wird oder nicht am Spiel teilnimmt. Wird mit jeder
		/// <code>CChosenRaces</code>- oder <code>CNextRound</code>-Nachricht
		/// aktualisiert und steht insbesondere zum Aufruf durch die Handler
		/// <code>CClientListener::OnClientChangedRaces()</code> und <code>CClientListener::OnNextRound()</code>
		// zur Verf�gung
		CString GetUserName(RACE nRace) const;

		/// @return Benutzername dieses Clients; <code>""</code>, wenn der Client nicht zu einem Server
		/// verbunden ist
		CString GetClientUserName() const {return m_strClientUserName;}

	private:
		/// Ruft <code>OnClientSocketError()</code> aller Listeners auf.
		void OnSocketError(int nError);

		/// Wartet, bis die angegebene Anzahl Bytes zur Verf�gung steht oder die gegebene
		/// Deadline abl�uft.
		static BOOL RequestData(CAsyncSocket &socket, UINT nCount, clock_t clkDeadline);

		/// F�hrt den client-seitigen Handshake aus. <code>nError</code> enth�lt bei Auftreten
		/// eines Socket-Fehlers den Fehlercode, sonst <code>0</code>
		BOOL DoClientHandshake(CSocket &socket, int &nError, const CString &strUserName);

		/// Versendet die angegebene Nachricht �ber die angegebene Verbindung.
		void Send(CConnection *pConnection, CMessage *pMessage);

		/// Vom Server eingetroffene <code>CChosenRaces</code>-Nachricht behandeln.
		void HandleChosenRaces(CChosenRaces *pMsg);
		/// <code>CBeginGame</code>-Nachricht vom Server verarbeiten.
		void HandleBeginGame(CBeginGame *pMsg);
		/// <code>CNextRound</code>-Nachricht vom Server verarbeiten.
		void HandleNextRound(CNextRound *pMsg);
		/// <code>CChatMsg</code>-Nachricht verarbeiten.
		void HandleChatMsg(CChatMsg *pMsg);

	};

};

/// globale Instanz
extern network::CBotEClient client;
