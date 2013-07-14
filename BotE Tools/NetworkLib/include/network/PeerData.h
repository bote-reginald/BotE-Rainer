#pragma once

#include "network.h"

namespace network
{
	/**
	 * Stellt Methoden f�r das Serialisieren und Deserialisieren der Spieldaten, die im Netzwerk
	 * �bertragen werden m�ssen, zur Verf�gung. Wird �blicherweise von der Dokumentenklasse des
	 * Servers und Clients implementiert.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CPeerData
	{
	public:
		/// Serialisiert die Daten, die zu Beginn eines Spieles vom Server einmal an alle Clients
		/// �bertragen werden (<code>CBeginGame</code>-Nachricht). (store: Server, load: Client)
		virtual void SerializeBeginGameData(CArchive &ar) =0;

		/// Serialisiert die Daten f�r die <code>CNextRound</code>-Nachricht. (store: Server, load: Client)
		virtual void SerializeNextRoundData(CArchive &ar) =0;

		/// Serialisiert die Daten f�r die <code>CEndOfRound</code>-Nachricht �ber das angegebene Volk.
		/// <code>race</code> wird beim Schreiben ignoriert. (store: Client, load: Server)
		virtual void SerializeEndOfRoundData(CArchive &ar, RACE race) =0;

	};

};
