#pragma once

#include "ReferenceCounter.h"

namespace network
{
	class CPacket;
};

#include "Message.h"

namespace network
{
	/**
	 * Basiseinheit, die auf einer Verbindung �bertragen wird. Enth�lt eingebettet die Daten
	 * eines <code>CMessage</code>-Objekts. Die Daten der Nachricht werden komprimiert
	 * �bertragen, wenn sie die L�nge <code>PACKET_COMPRESS</code> �berschreiten.
	 *
	 * Erlaubt durch Angabe einer Request-ID die Zuordnung von Responses zu einem zugeh�rigen Request.
	 *
	 * Aufbau eines Pakets:
	 * <pre>
	 * BYTE[]	capturePattern	Capture Pattern, == PACKET_CAPTURE_PATTERN, hat die L�nge PACKET_CAPTURE_LEN
	 * BYTE		flags			Flags, momentan nur PCK_COMPRESSED
	 * BYTE		id				ID der Nachricht
	 * int		requestId		ID des Requests/der Response, sonst -1
	 * UINT		size			L�nge von data (in Bytes)
	 * BYTE[]	data			Daten der Nachricht
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.6
	 */
	class CPacket : public CReferenceCounter
	{
	private:
		BYTE m_nFlags;			///< Flags (<code>PCK_COMPRESSED</code>)

		BYTE m_nID;				///< ID der zu �bertragenden Nachricht
		int m_nRequestId;		///< ID des Requests/der Response, <code>-1</code> wenn das Paket weder Request noch Response enth�lt
		UINT m_nSize;			///< L�nge des Puffers, der die Daten der Nachricht enth�lt
		BYTE *m_pData;			///< serialisierte Daten der zu �bertragenden Nachricht, evtl. komprimiert

	private:
		/**
		 * Erzeugt ein <code>CPacket</code>-Objekt, dessen Attribute noch sinnvoll gesetzt werden
		 * m�ssen. Zur Verwendung in <code>Receive()</code> gedacht.
		 */
		CPacket();

	public:
		/**
		 * Erzeugt ein Paket, dass die Daten der angegebenen Nachricht enth�lt. Die Nachricht
		 * wird mit Aufruf des Konstruktors serialisiert und ggf. komprimiert und kann ohne
		 * erneutes Serialisieren per <code>Send()</code> mehrfach versendet werden.
		 *
		 * @param pMessage die in das Paket zu packende Nachricht
		 * @param nRequestId ID des Requests (<code>&gt;= 0</code>), zu dem diese Nachricht (als Request
		 * oder Response) geh�rt; <code>-1</code>, wenn die Nachricht weder Request noch Response darstellt
		 */
		CPacket(CMessage *pMessage, int nRequestId = -1, BOOL bRequest = TRUE);
		virtual ~CPacket();

		int GetRequestId() const {return m_nRequestId;}
		BOOL IsRequest() const {return m_nRequestId >= 0 && ((m_nFlags & PCK_REQUEST) != 0);}
		BOOL IsResponse() const {return m_nRequestId >= 0 && ((m_nFlags & PCK_REQUEST) == 0);}

		/**
		 * Versendet die Nachricht �ber den angegebenen Socket.
		 *
		 * @param nError enh�lt nach dem Aufruf den von <code>socket.GetLastError()</code>
		 * gelieferten Fehler oder <code>0</code>, wenn kein Fehler aufgetreten ist
		 */
		BOOL Send(CAsyncSocket &socket, int &nError);

		/**
		 * Empf�ngt ein Paket vom angegebenen Socket. Die Daten der �bertragenen Nachricht
		 * werden weder dekomprimiert noch deserialisiert. Damit kann das Paket bei Bedarf
		 * effektiv weitergeleitet werden.
		 *
		 * @param nError enh�lt nach dem Aufruf den von <code>socket.GetLastError()</code>
		 * gelieferten Fehler oder <code>0</code>, wenn kein Fehler aufgetreten ist
		 * @param bClosed ist nach dem Aufruf <code>TRUE</code>, wenn die Verbindung von der
		 * anderen Seite aus beendet wurde, sonst <code>FALSE</code>
		 * @return das empfangene <code>CPacket</code>-Objekt oder <code>NULL</code>, wenn keine
		 * Daten im Empfangspuffer des Sockets vorhanden sind oder ein Fehler aufgetreten ist
		 */
		static CPacket *Receive(CAsyncSocket &socket, int &nError, BOOL &bClosed);

		/**
		 * Erzeugt das in den Daten des Pakets enthaltene <code>CMessage</code>-Objekt.
		 * Dekomprimiert die Daten, wenn diese komprimiert vorliegen und deserialisiert das
		 * <code>CMessage</code>-Objekt. Das gelieferte Objekt muss vom Aufrufer zerst�rt werden.
		 *
		 * @return das deserialisierte <code>CMessage</code>-Objekt oder <code>NULL</code>,
		 * wenn kein bekanntes <code>CMessage</code>-Objekt enthalten ist
		 */
		CMessage *GetMessage() const;

	private:
		/**
		 * Versendet die Daten im angegebenen Puffer. Kehrt erst zur�ck, wenn alle Daten
		 * versendet wurden oder ein Fehler auftrat.
		 * @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE</code>
		 */
		static BOOL SyncSend(CAsyncSocket &socket, const void *lpBuf, int nBufLen, int &nError, int nFlags = 0);

		/**
		 * Komprimiert die im angegebenen Puffer enthaltenen Daten, schreibt das Ergebnis nach out.
		 * @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE</code>
		 */
		static BOOL Compress(BYTE *lpBuf, UINT nBufLen, CFile &out);

		/**
		 * Dekomprimiert die im angegebenen Puffer enthaltnen Daten, schreibt das Ergebnis nach out.
		 * @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE</code>
		 */
		static BOOL Decompress(BYTE *lpBuf, UINT nBufLen, CFile &out);

	};

};
