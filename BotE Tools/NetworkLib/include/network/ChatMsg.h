#pragma once

#include "Message.h"

#pragma warning (disable: 4244)

namespace network
{
	typedef CList<RACE> RACE_LIST;

	/**
	 * Eine Chat-Nachricht. Wird von einem Client an den Server gesendet, der diese an die
	 * adressierten Empf�nger verteilt.
	 *
	 * Aufbau:
	 * <pre>
	 * CString	senderName		Name des Absenders; wird vom Server ggf. korrigiert
	 * BYTE		sender			Absender der Nachricht (RACE), w�hrend der Startphase immer RACE_NONE; wird vom Server
	 *							ggf. korrigiert
	 * BYTE		recvCount		Anzahl der Empf�nger, > 0
	 * BYTE[]	receivers		Empf�nger der Nachricht (RACE), recvCount Eintr�ge; ist einer der Eintr�ge
	 *							RACE_ALL, wird die Nachricht an alle verbundenen Clients gesendet (w�hrend
	 *							der Startphase also auch an Clients, denen kein Volk zugeordnet ist)
	 * CString	msg				Nachricht
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.2
	 */
	class CChatMsg : public CMessage
	{
	private:
		CString m_sSenderName;			///< Name des Absenders
		/// Volk des Absenders; beim Erzeugen immer <code>RACE_NONE</code>, wird vom Server korrekt ausgef�llt
		RACE m_nSender;
		/// Liste der Empf�nger der Nachricht; enth�lt entweder nur Eintr�ge <code>!= RACE_ALL</code> oder genau
		/// einen Eintrag mit dem Wert <code>RACE_ALL</code>
		RACE_LIST m_lReceivers;
		CString m_sMsg;					///< Nachricht

	public:
		CChatMsg();

		/// Erzeugt ein Chat-Nachrichtenobjekt. Whitespaces am Beginn und Ende der Nachricht werden abgeschnitten.
		CChatMsg(CString msg);
		/// Erzeugt ein Chat-Nachrichtenobjekt. Whitespaces am Beginn und Ende des Namens und der Nachricht
		/// werden abgeschnitten. Es wird der angegebene Empf�nger zur Liste der Empf�nger hinzugef�gt.
		CChatMsg(CString msg, RACE receiver);
		virtual ~CChatMsg();

		/// F�gt einen Empf�nger zur Liste der Empf�nger hinzu. Die Angabe von <code>RACE_ALL</code> ist m�glich,
		/// wobei alle anderen Eintr�ge entfernt werden.
		void AddReceiver(RACE race);
		/// Setzt das Volk des Absenders dieser Chatnachricht. Sendet der Client eine Chatnachricht an den Server,
		/// wird der Wert des Absendervolkes vom Server durch den tats�chlichen Wert ersetzt. W�hrend der
		/// Zuordnungsphase <code>RACE_NONE</code>.
		void SetSender(RACE nSender);
		/// Setzt den Benutzernamen des Absenders. Wird vom Server durch den Namen ersetzt, der beim Herstellen
		/// der Verbindung angegeben wurde.
		void SetSenderName(CString strSender);

		CString GetSenderName() const {return m_sSenderName;}
		RACE GetSenderRace() const {return m_nSender;}
		CString GetMessage() const {return m_sMsg;}
		UINT GetReceiverCount() const {return m_lReceivers.GetCount();}
		/// @return einen Empf�nger; <code>RACE_NONE</code>, wenn ein ung�ltiger Index angegeben wurde
		RACE GetReceiver(int idx) const;

		BOOL IsForAllClients() const;

		virtual void Serialize(CArchive &ar);

	};

};
