#pragma once
#include "Message.h"
#include "PeerData.h"

namespace network
{
	/**
	 * Nachricht, mit der ein Client seine Runde beendet und die Rundendaten an den Server sendet.
	 * Sind die <code>CEndOfRound</code>-Nachrichten aller Clients beim Server eingetroffen, berechnet
	 * dieser die Daten der n�chsten Runde und sendet eine <code>CNextRound</code>-Nachricht an alle
	 * Clients.
	 *
	 * Aufbau:
	 * <pre>
	 * UINT		size			L�nge von data
	 * BYTE[]	data			serialisierte Daten des Dokuments
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CEndOfRound :	public CMessage
	{
	private:
		UINT m_nSize;									///< Gr��e des Puffers <code>m_pData</code>
		BYTE *m_pData;									///< serialisierte Daten des Dokuments

	public:
		CEndOfRound();

		/**
		 * Erzeugt die Nachricht und serialisiert die Daten des angegebenen Client-Dokuments per
		 * <code>CPeerData::SerializeEndOfRoundData()</code>.
		 */
		CEndOfRound(CPeerData *pDoc);
		virtual ~CEndOfRound();

		virtual void Serialize(CArchive &ar);

		/// @return L�nge des Puffers, der die serialisierten Daten enth�lt
		UINT GetSize() const {return m_nSize;}
		/// Liefert den Puffer mit den serialisierten Daten der Nachricht und koppelt ihn von diesem
		/// Nachrichtenobjekt ab. Der Puffer wird damit nicht mehr freigegeben und muss sp�ter vom
		/// Benutzer freigegeben werden.
		BYTE *Detach();

	};

};
