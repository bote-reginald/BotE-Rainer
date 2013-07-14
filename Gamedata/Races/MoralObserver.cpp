#include "stdafx.h"
#include "MoralObserver.h"
#include "IOData.h"

IMPLEMENT_SERIAL (CMoralObserver, CObject, 1)
// statische Variable initialisieren
short CMoralObserver::m_iMoralMatrix[][MAJOR6] = {0};
CString CMoralObserver::m_strTextMatrix[][MAJOR6] = {""};

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMoralObserver::CMoralObserver(void)
{
}

CMoralObserver::~CMoralObserver(void)
{
	m_iEvents.RemoveAll();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMoralObserver::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		m_iEvents.Serialize(ar);
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		m_iEvents.RemoveAll();
		m_iEvents.Serialize(ar);
	}
}

/// Serialisierungsfunktion f�r statische Variable
void CMoralObserver::SerializeStatics(CArchive &ar)
{
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i < EVENTNUMBER; i++)
			for (int j = 0; j < MAJOR6; j++)
			{
				ar << m_iMoralMatrix[i][j];
				//MYTRACE("logsave")(MT::LEVEL_DEBUG, "m_iMoralMatrix: %d %d Value: %i\n", i, j, m_iMoralMatrix);
				ar << m_strTextMatrix[i][j];
			}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		for (int i = 0; i < EVENTNUMBER; i++)
			for (int j = 0; j < MAJOR6; j++)
			{
				ar >> m_iMoralMatrix[i][j];
				ar >> m_strTextMatrix[i][j];
			}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion berechnet die Moralauswirkung auf alle Systeme, abh�ngig von den vorgekommenen Events und
/// der jeweiligen Majorrace. �bergeben werden daf�r alle Systeme <code>systems</code>, die RaceID und die
/// gemappte Nummer der Majorrace <code>byMappedRaceNumber</code>, auf welche Moralwerte sich bezogen werden soll.
void CMoralObserver::CalculateEvents(std::vector<CSystem>& systems, const CString& sRaceID, BYTE byMappedRaceNumber)
{
	// derzeit nur 6 Moralwertdatens�tze vorhanden
	ASSERT(byMappedRaceNumber >= 1 && byMappedRaceNumber <= 6);


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
													Fed	Fer	Kli	Rom	Car	Dom
#0	Eliminate an Empire								50	50	50	50	50	50		-> eingebaut
#1	Win a Major Battle								5	5	7	6	4	5
#2	Win a Significant Battle						3	3	5	4	2	2
#3	Win a Minor Battle								1	1	2	1	1	1		-> eingebaut (vereinfacht, immer Minorbattle)
#4	Lose a Major Battle								-7	-7	-13	-11	-6	-7
#5	Lose a Significant Battle						-7	-6	-10	-9	-4	-5
#6	Lose a Minor Battle								-2	-2	-4	-4	-2	-2		-> eingebaut (vereinfacht)
#7	Lose a Flagship									-10	-9	-13	-10	-8	-8		-> eingebaut
#8	Lose an Outpost									-3	-5	-1	-4	-1	-1		-> eingebaut
#9	Lose a Starbase�								-4	-5	-2	-5	-2	-2		-> eingebaut

#10	Sign a Membership treaty with Minor				12	6	6	6	4	4		-> eingebaut

#11	Take a System									-8	2	10	4	8	8		-> eingebaut
#12	Colonize a System								3	3	4	4	2	3		-> eingebaut
#13	Liberate a Minor Race System					20	1	-1	2	-5	-5		-> eingebaut
#14	Liberate a Former Native System					9	5	6	10	5	4		-> eingebaut
#15	Lose Home System to Outside Forces				-20	-20	-20	-20	-20	-30		-> eingebaut
#16	Lose a Member System to Outside Forces			-10	-10	-10	-10	-7	-10		-> eingebaut
#17	Lose a Subjugated System to Outside Forces�		-5	-7	-10	-10	-7	-10		-> eingebaut
#18	Lose a System to Rebellion						-15	-10	-10	-15	-5	-10		-> eingebaut
#19	Bombard a System								-5	-2	6	4	4	2		-> eingebaut

#20	Bombard native system that has Rebelled			-7	-2	5	3	5	5		-> eingebaut
#21	Eliminate a Minor Race Entirely					-20	-15	2	-4	4	5		-> eingebaut
#22	Suffer bombardment of System					-1	-2	1	-1	2	-1		-> eingebaut
#23	Suffer 50% pop loss in Home Sys. Bombardment	-10	-10	-10	-10	-10	-10

#24	Declare War on an Empire when Neutral			-5	1	7	1	3	3		-> eingebaut
#25	Declare War on an Empire when Non-Aggression	-6	0	6	1	3	2		-> eingebaut
#26	Declare War on an Empire with Trade Treaty		-7	-10	4	1	3	1		-> eingebaut
#27	Declare War on an Empire with Friendship Treaty	-9	-2	-2	-2	2	1		-> eingebaut
#28	Declare War on an Empire with Defense Pact		-10	-3	-2	-4	0	1		-> eingabaut
#29	Declare War on an Empire with CooperationTreaty -10	-4	-2	-6	-4	1		-> eingebaut
#30	Declare War on an Empire with Affiliation		-12	-8	-5	-10	-5	1		-> eingebaut
#31	Other Empire Declares War when Neutral			5	2	2	5	3	1		-> eingebaut
#32	Other Empire Declares War with Treaty			7	4	4	6	4	1		-> eingebaut
#33	Other Empire Declares War with an Affiliation	10	8	7	8	6	2		-> eingebaut

#34	Sign Trade Treaty								3	5	0	2	1	-3		-> eingebaut
#35	Sign Friendship/Cooperation Treaty				5	3	1	4	2	-6		-> eingebaut
#36	Sign an Affiliation Treaty						8	5	5	5	4	4		-> eingebaut

#37	Receive Acceptance of Non-Aggression Pact		2	1	-3	0	-1	-4
#38	Accept a Non-Aggression Pact					2	1	-4	-1	-2	-4
#39	Receive Acceptance of Defence Pact				5	2	-6	1	-2	-8
#40	Accept a Defence Pact							5	2	-8	-2	-4	-8
#41	Receive Acceptance of War Pact					-2	3	8	3	4	2
#42	Accept a War Pact								-6	1	6	2	4	2

#43	Refuse a Treaty (Trade)							-3	-5	0	1	-1	3
#44	Refuse a Treaty (Friendship, Coop)				-5	-2	1	2	-2	5
#45	Refuse a Treaty (Affiliation)					-10	-4	2	4	-4	10
#46	Refuse a Treaty (Non_Aggression, Defence Pact)	-5	-2	6	2	0	5
#47	Refuse a Treaty (Warpact)						5	0	-10	0	-2	0

#48	Accept a Request								0	-5	-4	-2	-1	-3
#49	Refuse a Request								0	2	2	0	0	1
#50	Receive Acceptance of a Request					1	5	4	1	1	1

#51	Accept Surrender								-50	-50	-50	-50	-50	-50
#52	Receive a Surrender								20	20	20	20	20	10
#53	Accept 'Victory' Non-Aggression Treaty			2	-2	-12	-6	-5	-12
#54	Receive Acceptance of Victory Treaty Demand		8	8	10	6	8	8

#55	Accept a System's Request for Independence		-6	-9	-5	-15	-9	-15
#56	Refuse a System's Request for Independence		-4	2	2	2	5	2

#57	Repel a Borg Attack								10	6	6	12	8	3

#58	Lose population of system to natural event		-2	-2	-2	-2	-2	-1
#59 Successful espionage							0	0	0	1	1	1		-> eingebaut
#60 Successful sabotage								0	0	-1	2	2	1		-> eingebaut
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	short moralMatrix[][MAJOR6] = {
		50,	50,	50,	50,	50,	50,		// #0	Eliminate an Empire
		5,	5,	7,	6,	4,	5,
		3,	3,	5,	4,	2,	2,
		1,	1,	2,	1,	1,	1,
		-7,	-7,	-13,-11,-6,	-7,
		-7,	-6,	-10,-9,	-4,	-5,
		-2,	-2,	-4,	-4,	-2,	-2,
		-10,-9,	-13,-10,-8,	-8,
		-3,	-5,	-1,	-4,	-1,	-1,
		-4,	-5,	-2,	-5,	-2,	-2,		// #9	Lose a Starbase
		12,	6,	2,	5,	4,	4,		// #10	Sign a Membership treaty with Minor
		-8,	2,	10,	4,	8,	8,
		3,	3,	4,	4,	2,	3,
		20,	1,	-1,	2,	-5,	-5,
		9,	5,	6,	10,	5,	4,
		-20,-20,-20,-20,-20,-30,
		-10,-10,-10,-10,-7,	-10,
		-5,	-7,	-10,-10,-7,	-10,
		-15,-10,-10,-15,-5,	-10,
		-5,	-2,	6,	4,	4,	2,		// #19	Bombard a System if you've declared war
		-7,	-2,	5,	3,	5,	5,
		-20,-15,2,	-4,	4,	5,
		-1,	-2,	1,	-1,	2,	-1,
		-10,-10,-10,-10,-10,-10,		// #23	Suffer 50% pop loss in Home Sys. Bombardment
		-5,	1,	7,	1,	3,	3,
		-6,	0,	6,	1,	3,	2,
		-7,	-10,4,	1,	3,	1,
		-9,	-2,	-2,	-2,	2,	1,
		-10,-3,	-2,	-4,	0,	1,
		-10,-4,	-2,	-6,	-4,	1,
		-12,-8,	-5,	-10,-5,	1,
		5,	2,	2,	5,	3,	1,
		7,	4,	4,	6,	4,	1,
		10,	8,	7,	8,	6,	2,		// #33	Other Empire Declares War with an Affiliation
		3,	5,	0,	2,	1,	-3,
		5,	3,	1,	4,	2,	-6,
		20,	16,	16,	16,	10,	4,		// #36	Sign an Affiliation Treaty
		2,	1,	-3,	0,	-1,	-4,
		2,	1,	-4,	-1,	-2,	-4,
		5,	2,	-6,	1,	-2,	-8,
		5,	2,	-8,	-2,	-4,	-8,
		-2,	3,	8,	3,	4,	2,
		-6,	1,	6,	2,	4,	2,		// #42	Accept a War Pact
		-3,	-5,	0,	1,	-1,	3,
		-5,	-2,	1,	2,	-2,	5,
		-10,-4,	2,	4,	-4,	10,
		-5,	-2,	6,	2,	0,	5,
		5,	0,	-10,0,	-2,	0,		// #47	Refuse a Treaty (Warpact)
		0,	-5,	-4,	-2,	-1,	-3,
		0,	2,	2,	0,	0,	1,
		1,	5,	4,	1,	1,	1,		// #50	Receive Acceptance of a Request
		-50,-50,-50,-50,-50,-50,
		20,	20,	20,	20,	20,	10,
		2,	-2,	-12,-6,	-5,	-12,
		8,	8,	10,	6,	8,	8,		// #54	Receive Acceptance of Victory Treaty Demand
		-6,	-9,	-5,	-15,-9,	-15,
		-4,	2,	2,	2,	5,	2,
		10,	6,	6,	12,	8,	3,
		-2,	-2,	-2,	-2,	-2,	-1,		// #58	Lose population of system to natural event
		0,	0,	0,	1,	1,	1,		// #59	Successful espionage
		0,	0,	-1,	2,	2,	1		// #60	Successful sabotage
	};
*/
	for (int i = 0; i < m_iEvents.GetSize(); )
	{
		short moral = m_iMoralMatrix[m_iEvents.GetAt(i)][byMappedRaceNumber - 1];
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem() == sRaceID)
					systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).SetMoral(moral);
		m_iEvents.RemoveAt(i);
	}
}

/// Funktion erstellt einen Text, der in der Ereignisansicht angezeigt werden kann, aufgrund eines speziellen
/// Events. Dieser Text ist rassenabh�ngig und wird von der Funktion zur�ckgegeben. Als Parameter werden an diese
/// Funktion das Event <code>event</code> und die Nummer der Majorrace <code>major</code> �bergeben. Optional muss
/// auch ein Textstring <code>param</code> �bergeben werden, der in bestimmte Nachrichten eingebaut wird.
CString CMoralObserver::GenerateText(unsigned short Event, BYTE major, const CString& param) const
{
/*	CString textMatrix[][MAJOR6] = {
	// #0	Eliminate an Empire
		"","","","","","",
	// #1	Win a Major Battle
		"Unsere B�rger sind sehr gl�cklich �ber den Ausgang der Schlacht",
		"H�ndler und Investoren sind �ber den Ausgang der Schlacht erfreut",
		"Krieger feiern ausgelassen unseren Erfolg in der Schlacht",
		"Die Bev�lkerung feiert unseren Sieg in der Schlacht",
		"Das Volk jubelt �ber unseren milit�rischen Erfolg in der Schlacht",
		"Die Gr�nder sind mit dem Verlauf der Schlacht zufrieden",
	// #2	Win a Significant Battle
		"Unsere B�rger sind erfreut �ber den Ausgang des Kampfes",
		"H�ndler und Investoren sind �ber den Ausgang des Kampfes erfreut",
		"Krieger feiern unseren Erfolg im Kampf",
		"Die Bev�lkerung freut sich �ber unseren Sieg im Kampf",
		"Das Volk freut sich �ber unseren milit�rischen Erfolg im Kampf",
		"Die Gr�nder sind mit dem Ausgang des Kampfes zufrieden",
	// #3	Win a Minor Battle
		"Unsere B�rger haben den Ausgang des Kampfes positiv aufgenommen",
		"H�ndler und Investoren investieren verst�rkt in das Milit�r",
		"Krieger freuen sich �ber unseren Erfolg im Kampf",
		"Die Bev�lkerung hat unseren Sieg im Kampf sehr positiv aufgenommen",
		"Das Volk hat unseren milit�rischen Erfolg im Kampf positiv aufgenommen",
		"Die Gr�nder wurden mit dem Ausgang des Kampfes zufriedengestellt",
	// #4	Lose a Major Battle
		"Unsere B�rger sind sehr traurig �ber Verluste in der Schlacht",
		"H�ndler und Investoren sind �ber unserer Niederlage schockiert",
		"Krieger sind wegen der Niederlage in der Schlacht extrem schockiert",
		"Die Bev�lkerung hat unser Versagen in der Schlacht scharf verurteilt",
		"Das Volk hat wegen der Niederlage den Glauben an die Union verloren",
		"Die Gr�nder sind wegen der Niederlage sehr zornig",
	// #5	Lose a Significant Battle
		"Unsere B�rger sind traurig �ber unsere Verluste im Kampf",
		"H�ndler und Investoren halten Gelder wegen der Niederlage zur�ck",
		"Krieger sind wegen der Niederlage im Kampf schockiert",
		"Die Bev�lkerung ist wegen unserem Versagen emp�rt",
		"Das Volk versteht unsere Niederlage im Kampf nicht",
		"Die Gr�nder sind wegen der Niederlage zornig",
	// #6	Lose a Minor Battle
		"Unsere B�rger haben den Ausgang des Kampfes negativ aufgenommen",
		"H�ndler und Investoren investieren weniger in das Milit�r",
		"Krieger sind zornig �ber unsere Niederlage im Kampf",
		"Die Bev�lkerung hat unser Versagen im Kampf negativ aufgenommen",
		"Das Volk hat unsere milit�rische Niederlage im Kampf negativ aufgenommen",
		"Die Gr�nder wurden mit dem Ausgang des Kampfes nicht zufriedengestellt",
	// #7	Lose a Flagship
		"B�rger sind �ber den Verlust unseres Flagschiffes "+param+" schockiert",
		"Investoren haben durch den Verlust des Flagschiffes "+param+" Gelder verloren",
		"Krieger wollen den Verlust des Flagschiffes "+param+" nicht wahrhaben",
		"Die Bev�lkerung ist �ber den Verlust des Flagschiffes "+param+" emp�rt",
		"Das Volk ist �ber den Verlust des Flagschiffes "+param+" schockiert",
		"Die Gr�nder haben den Verlust des Flagschiffes "+param+" mit Grollen aufgenommen",
	// #8	Lose an Outpost
		"B�rger sind �ber den Verlust eines Aussenpostens traurig",
		"Investoren haben durch den Verlust eines Aussenpostens Gelder verloren",
		"Krieger wollen den Verlust eines Aussenpostens nicht wahrhaben",
		"Die Bev�lkerung ist �ber den Verlust eines Aussenpostens emp�rt",
		"Das Volk hat den Verlust eines Aussenpostens negativ aufgenommen",
		"Die Gr�nder haben den Verlust eines Aussenpostens bemerkt",
	// #9	Lose a Starbase
		"B�rger sind �ber den Verlust einer Sternbasis sehr traurig",
		"Investoren haben durch den Verlust ihrer Sternbasis viel Geld verloren",
		"Krieger wollen den Verlust einer Sternbasis nicht wahrhaben",
		"Die Bev�lkerung ist �ber den Verlust einer Sternbasis emp�rt",
		"Das Volk hat den Verlust einer Sternbasis negativ aufgenommen",
		"Die Gr�nder haben den Verlust einer Sternbasis bemerkt",
	// #10	Sign a Membership treaty with Minor
		"B�rger freuen sich �ber den Beitritt der "+param+" in die F�deration",
		"H�ndler und Investoren freuen sich auf neue M�rkte durch die "+param,
		"Krieger sind Stolz auf die "+param+" als neue Br�der",
		"Die Bev�lkerung freut sich �ber den Beitritt der "+param+" zum Sternenimperiums",
		"Das Volk freut sich �ber den Beitritt der "+param+" zur Union",
		"Die Gr�nder haben die Mitgliedschaft der "+param+" positiv bemerkt",
	// #11	Take a System
		"Die B�rger heissen die Eroberung des "+param+"-Systems nicht gut",
		"H�ndler und Investoren freuen sich auf neue M�rkte im "+param+"-System",
		"Unsere Krieger feiern die ruhmreiche Eroberung des "+param+"-Systems",
		"Das Volk begr�sst die gewaltsame Eroberung des "+param+"-Systems",
		"Die Regierung freut sich auf neue Sklaven aus dem "+param+"-System",
		"Die Gr�nder haben die Eroberung des "+param+"-System mit Wohlwollen aufgenommen",
	// #12	Colonize a System
		"Die F�deration feiert Kolonisierung des Systems "+param,
		"Begeisterung: neuer Handel in der Kolonie auf "+param,
		"Das System "+param+" gibt unserem Volk Raum zum Atmen",
		"Das Volk ist froh, dass "+param+" ein Teil des Imperiums ist",
		"Unionsb�rger feierten die Kolonialisierung von "+param,
		"Die Gr�nder geben ihre Zustimmung zu Kolonisierung des "+param+"-Systems",
	// #13	Liberate a Minor Race System
		"Paraden werden zu Ehren jener abgehalten, die die "+param+" befreiten",
		"Die "+param+" k�nnten wieder mit uns handeln",
		"Die "+param+" wurden befreit: Unsere Krieger sind trotzdem nicht stolz",
		"Die "+param+" wurden befreit: Das Vertrauen ist wiederhergestellt",
		"Die Bev�lkerung ist nicht erfreut, dass "+param+" befreit wurde",
		"Die "+param+" wurden befreit: Gr�nder zweifeln an dieser Taktik",
	// #14	Liberate a Former Native System
		"Die Befreiung des "+param+"-Systems sorgt f�r Unterst�tzung",
		"Der neue "+param+"-Markt ist gut f�r Exporteure",
		"Die Befreiung von "+param+" bringt Krieger zum Bat'leth-Orden",
		"Die Rettung von "+param+" ist willkommener Anlass zum Feiern",
		"Die Befreiung von "+param+" erfreut die loyalen Cardassianer",
		"Die Befreiung von "+param+" erfreut die Gr�nder",
	// #15	Lose Home System to Outside Forces
		"Demonstranten fordern Erkl�rung: warum wurde das Sol-System verloren?",
		"Der Verlust von Ferenginar sorgte f�r das gr�sste B�rsentief",
		"Leben ohne Qo'nos verursacht Selbstmord unter den Kriegern",
		"Das Volk: Verlust von Romulus ist eine Inkompetenz der Regierung",
		"Verlust von Cardassia Prime f�hrt zu Trauer und Selbstmorden",
		"Verlust der Gr�nderwelt: Gr�nder sehen in ihrem Dasein keinen Sinn mehr",
	// #16	Lose a Member System to Outside Forces
		"Die Bev�lkerung ist �ber den Verlust von "+param+" erbost",
		"Das Anlegervertrauen ist durch den Verlust von "+param+" leicht gesunken",
		"Krieger sind besch�mt �ber das Versagen bei der Verteidigung von "+param,
		"Volk hat Entt�uschung �ber den Verlust von "+param+" ausgedr�ckt",
		"Die Cardassianer bedauern den Verlust des Systems "+param,
		"Gr�nder k�nnen den Verlust von "+param+" noch immer nicht glauben",
	// #17	Lose a Subjugated System to Outside Forces
		"Das "+param+"-System konnte nicht rechtm�ssig verteidigt werden",
		"B�rger des "+param+"-Systems sind keine Kunden mehr. Das ist schlecht f�r den Export",
		"Wir konnten das "+param+"-System nicht verteidigen und verloren viel Ehre",
		"Wir konnten "+param+"-System nicht besch�tzen",
		"Das cardassianische Volk betrauert, dass "+param+"-System verloren ist",
		"Verlust des "+param+"-Systems erz�rnt die Gr�nder",
	// #18	Lose a System to Rebellion
		"B�rger in der ganzen F�deration verurteilen die Rebellion im "+param+"-System",
		"H�ndler und Investoren sind ungl�cklich �ber die Rebellion "+param+"-System",
		"Krieger empfinden Wut wegen der Rebellion im "+param+"-System",
		"Das Volk kann die Rebellion im "+param+"-System absolut nicht nachvollziehen",
		"B�rger der Union haben die Rebellion im "+param+"-System negativ aufgenommen",
		"Rebellion im "+param+"-System hat die Gr�nder sehr ver�rgert",
	// #19	Bombard a System
		"Angriff auf "+param+" verringerte �ffentliche Unterst�tzung",
		"Bombardierung des "+param+"-Systems verhindert B�rsenaufschwung",
		"Bombardierung von "+param+" zeigt, wir sind  Klingonen",
		"Bombardierung des "+param+"-Systems verstummt Dissidenten",
		"Bombardierung des "+param+"-Systems verstummte Unionskritiker",
		"Bombardierung des "+param+"-Systems l�sst die Gr�nder aufhorchen",
	// #20	Bombard native system that has Rebelled
		"Angriff auf die Rebellen im "+param+"-System wird vom Volk nicht unterst�tzt",
		"Bombardierung der Rebellen im "+param+"-System ist schlecht f�r die B�rse",
		"Bombardierung unser rebellischen Br�der im "+param+"-System erfreut die wahren Klingonen",
		"Bombardierung der Rebellen im "+param+"-System kann leicht vertuscht werden",
		"Bombardierung der Rebellen im "+param+"-System erfreut die Union",
		"Die Gr�nder rechtfertigen die Bombardierung der Rebellen im "+param+"-System",
	// #21	Eliminate a Minor Race Entirely
		"Demonstration: Ausl�schung der "+param+" kann nicht nachvollzogen werden",
		"Ausl�schung der "+param+": B�rse reagiert mit enormen Kursverlusten",
		"Klingonen feiern die Ausl�schung der "+param,
		"Volk verurteilt die Ausl�schung der "+param,
		"Ausl�schung der "+param+" gerechtfertigt: Sie waren es nicht wert",
		"Vernichtung der "+param+" erfreut die Gr�nder",
	// #22	Suffer bombardment of System
		"Volk fordert Beendung der Bombardierung von "+param,
		"Bombardierung von "+param+" zerst�rt Gesch�fte",
		"Klingonen fordern: Helft unseren Br�dern auf "+param,
		"Volk bittet den Seant endlich gegen die Bombardierung von "+param+" einzuschreiten",
		"B�rger der Union haben kein Mitleid mit der Bev�lkerung auf "+param,
		"Gr�nder fordern die sofortige Beendung des Bombardements auf "+param,
	// #23	Suffer 50% pop loss in Home Sys. Bombardment
		"","","","","","",
	// #24	Declare War on an Empire when Neutral
		"Unser Krieg gegen "+param+" st�sst auf starke Kritik",
		"Positive Reaktion auf die Kriegserkl�rung an "+param,
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Das Volk freut sich, dass wir "+param+" bek�mpfen k�nnen",
		"Unser Volk begr�sst den Krieg gegen "+param+" sehr",
		"Die Gr�nder freuen sich auf den Krieg gegen "+param+" sehr",
	// #25	Declare War on an Empire when Non-Aggression
		"Unser Krieg gegen "+param+" st�sst auf starke Kritik",
		"Geteilte Reaktionen auf die Kriegserkl�rung an "+param,
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Das Volk freut sich, dass wir "+param+" bek�mpfen k�nnen",
		"Unser Volk begr�sst den Krieg gegen "+param+" sehr",
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #26	Declare War on an Empire with Trade Treaty
		"Unser Krieg mit "+param+" st�sst auf sehr starke Kritik",
		"Extrem negative Reaktionen auf die Kriegserkl�rung an "+param,
		"Unser Krieg gegen "+param+" erfreut manche Krieger",
		"Das Volk freut sich, dass wir "+param+" vernichten werden",
		"Unser Volk begr�sst den Krieg gegen "+param+" sehr",
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #27	Declare War on an Empire with Friendship Treaty
		"Unser Krieg gegen "+param+" st�sst auf sehr starke Kritik",
		"Negative Reaktionen auf die Kriegserkl�rung an "+param,
		"Unser Krieg gegen "+param+" wird von den wahren Kriegern nicht verstanden",
		"Das Volk ist gegen einen Krieg gegen "+param,
		"Unser Volk begr�sst den Krieg gegen "+param,
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #28	Declare War on an Empire with Defense Pact
		"Unser Krieg gegen "+param+" st�sst auf extrem starke Kritik",
		"Negative Reaktionen auf den Krieg gegen "+param,
		"Unser Krieg gegen "+param+" wird von den wahren Kriegern nicht verstanden",
		"Das Volk ist sehr gegen den Krieg gegen "+param,
		"Unser Volk hat �ber den Krieg gegen "+param+" eine geteilte Meinung",
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #29	Declare War on an Empire with CooperationTreaty
		"Unser Krieg gegen "+param+" st�sst auf extrem starke Kritik",
		"Sehr negative Reaktionen auf den Krieg gegen "+param,
		"Unser Krieg gegen "+param+" wird von den wahren Kriegern nicht verstanden",
		"Das gesamte Volk ist geschlossen gegen den Krieg gegen "+param,
		"Unser Volk verabscheut den Krieg gegen "+param,
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #30	Declare War on an Empire with Affiliation
		"Unser Krieg gegen "+param+" l�sst das Volk auf die Barrikaden gehen",
		"Extrem negative Reaktionen auf den Krieg gegen "+param,
		"Unser Krieg gegen "+param+" wird von allen Kriegern abgelehnt",
		"Das gesamte Volk �bt starke Kritik wegen dem Krieg gegen "+param,
		"Unser Volk verabscheut den Krieg gegen "+param,
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #31	Other Empire Declares War when Neutral
		"Krieg gegen "+param+" hat Unterst�tzung",
		"Krieg gegen "+param+" l�sst auf ein gutes Quartal hoffen",
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Ein Krieg gegen "+param+" bedeutet Abrechnung",
		"Durch Krieg gegen "+param+" ist die Zahl der freiwilligen drastisch gestiegen",
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #32	Other Empire Declares War with Treaty
		"Krieg gegen "+param+" hat grosse Unterst�tzung",
		"Krieg gegen "+param+" l�sst auf ein sehr gutes Quartal hoffen",
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Ein Krieg gegen "+param+" bedeutet Abrechnung",
		"Durch Krieg gegen "+param+" ist die Zahl der freiwilligen drastisch gestiegen",
		"Die Gr�nder freuen sich auf den Krieg gegen "+param,
	// #33	Other Empire Declares War with an Affiliation
		"Krieg gegen "+param+" hat sehr grosse Unterst�tzung",
		"Krieg gegen "+param+" l�sst auf viele sehr gutes Quartale hoffen",
		"Unser Krieg gegen "+param+" erfreut alle wahren Krieger sehr",
		"Ein Krieg gegen "+param+" bedeutet Abrechnung: Volk feiert",
		"Durch Krieg gegen "+param+" ist die Zahl der freiwilligen drastisch gestiegen",
		"Die Gr�nder freuen sich sehr auf den Krieg gegen "+param,
	// #34	Sign Trade Treaty
		"Neuer Handel mit "+param+" stimmt B�rger fr�hlich",
		"H�ndler und Investoren freuen sich auf den Handel mit "+param,
		"Handelsvertrag mit "+param+" interessiert die wahren Krieger kaum",
		"Das Volk ist �ber den Handelsvertrag mit "+param+" froh",
		"B�rger finden den Handelsvertrag mit "+param+" gut",
		"Gr�nder haben kein Verst�ndnis f�r den Handelsvertrag mit "+param,
	// #35	Sign Friendship/Cooperation Treaty
		"B�rger sind gl�cklich �ber den neuen Vertrag mit "+param+": Breite Zustimmung",
		"Der neue Vertrag mit "+param+" ist gut f�r das Gesch�ft",
		"Krieger verstehen die Unterzeichnung des Vertrages mit "+param,
		"Das Volk ist �ber den Vertrag mit "+param+" gl�cklich",
		"B�rger der Union finden den Vertrag mit "+param+" gut",
		"Gr�nder haben absolut kein Verst�ndnis f�r den Vertrag mit "+param,
	// #36	Sign an Affiliation Treaty
		"B�rger feiern ausgelassen das neue B�ndnis mit "+param,
		"H�ndler feiern ausgelassen das neue B�ndnis mit "+param,
		"Krieger feiern ausgelassen das neue B�ndnis mit "+param,
		"Das Volk feiert ausgelassen das neue B�ndnis mit "+param,
		"B�rger sind gl�cklich �ber das neue B�ndnis mit "+param,
		"Gr�nder zeigen positive Reaktionen auf das B�ndnis mit "+param,
	// #37 -> #58
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
	// #59	Successful espionage
		"B�rger sind die Spionageaktionen unseres Geheimdienstes egal",
		"H�ndler konzentrieren sich lieber auf Gesch�fte als auf Spionage",
		"Krieger bezeichnen Spionageaktionen als unehrenhaft",
		"Das Volk findet die Spionage des Tal'Shiar gut",
		"B�rger der Union finden leichten Gefallen an den Spionageaktionen",
		"Gr�nder nehmen die Spionageaktionen mit einem Schmunzeln zur Kenntniss",
	// #60	Successful sabotage
		"B�rger sind die Sabotageaktionen unseres Geheimdienstes egal",
		"H�ndler konzentrieren sich lieber auf Gesch�fte als auf Sabotage",
		"Krieger finden feige Saobtageaktionen als unehrenhaft",
		"Das Volk begr�sst die Sabotageaktionen des Tal'Shiar",
		"B�rger der Union finden Gefallen an den Sabotageaktionen des Ordens",
		"Gr�nder nehmen die Sabotageaktionen mit einem Schmunzeln zur Kenntniss"
	};
	*/
	CString text = m_strTextMatrix[Event][major-1];
	text.Replace("$param$", param);

	int nMoralValue = GetMoralValue(major, Event);
	CString s = "";
	if (nMoralValue > 0)
		s.Format(" (+%d)", nMoralValue);
	else if (nMoralValue < 0)
		s.Format(" (%d)", nMoralValue);
	else
		s = " (+- 0)";

	text += s;
	return text;
}
/*													Fed	Fer	Kli	Rom	Car	Dom
#37	Receive Acceptance of Non-Aggression Pact		2	1	-3	0	-1	-4
#38	Accept a Non-Aggression Pact					2	1	-4	-1	-2	-4	// KLI: Nichtangriffspakt mit "+param+" angenommen: Ent�uschung" ROM: Nichtangriffspakt mit "+param+": Breite Zustimmung"
#39	Receive Acceptance of Defence Pact				5	2	-6	1	-2	-8
#40	Accept a Defence Pact							5	2	-8	-2	-4	-8
#41	Receive Acceptance of War Pact					-2	3	8	3	4	2
#42	Accept a War Pact								-6	1	6	2	4	2

#43	Refuse a Treaty (Trade)							-3	-5	0	1	-1	3
#44	Refuse a Treaty (Friendship, Coop)				-5	-2	1	2	-2	5
#45	Refuse a Treaty (Affiliation)					-10	-4	2	4	-4	10
#46	Refuse a Treaty (Non_Aggression, Defence Pact)	-5	-2	6	2	0	5	// KLI: Unser Volk ist froh, dass der Frieden abgewendet werden konnte ROM: Frieden mit "+param+" abgelehnt: das Volk ist gl�cklich"
#47	Refuse a Treaty (Warpact)						5	0	-10	0	-2	0

#48	Accept a Request								0	-5	-4	-2	-1	-3
#49	Refuse a Request								0	2	2	0	0	1
#50	Receive Acceptance of a Request					1	5	4	1	1	1

#51	Accept Surrender								-50	-50	-50	-50	-50	-50
#52	Receive a Surrender								20	20	20	20	20	10
#53	Accept 'Victory' Non-Aggression Treaty			2	-2	-12	-6	-5	-12
#54	Receive Acceptance of Victory Treaty Demand		8	8	10	6	8	8

#55	Accept a System's Request for Independence		-6	-9	-5	-15	-9	-15
#56	Refuse a System's Request for Independence		-4	2	2	2	5	2

#57	Repel a Borg Attack								10	6	6	12	8	3

#58	Lose population of system to natural event		-2	-2	-2	-2	-2	-1
*/

/// Funktion l�dt die zu Beginn die ganzen Moralwerte f�r alle verschiedenen Ereignisse in eine Moralmatrix.
void CMoralObserver::InitMoralMatrix()
{
	// Moralwerte laden
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Other\\Moral.data";
	CStdioFile file;
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		CString input;
		int i = 0;
		while (file.ReadString(input))
		{
			int pos = input.ReverseFind(':');
			input.Delete(0, pos + 1);
			input.Trim();
			pos = 0;
			for (int j = 0; j <= MAJOR6; j++)
			{
				CString value = input.Tokenize("\t", pos);
				// (funktioniert) MYTRACE("init")(MT::LEVEL_DEBUG, "Moral.data: %s\n", value);
				m_iMoralMatrix[i][j] = atoi(value);
			}
			i++;
		}
	}
	else
	{
		AfxMessageBox("Error! Could not open \"Moral.data\"...");
		exit(1);
	}
	file.Close();

	// Textnachrichten laden
	fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Other\\MoralEvents.data";
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		CString input;
		int i = -1;
		int race = 0;
		while (file.ReadString(input))
		{
			// jedes neue Event beginnt mit einer Nummer, z.B. #17
			if (input.Left(1) != "#")
			{
				input.Trim();
				m_strTextMatrix[i][race++] = input;
			}
			else
			{
				i++;
				race = 0;
			}
		}
	}
	else
	{
		AfxMessageBox("Error! Could not open \"MoralEvents.data\"...");
		exit(1);
	}
	file.Close();

/*
	fileName = *((CBotEApp*)AfxGetApp())->GetPath() + "Data\\Other\\MoralEventsTest.txt";
	if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		for (int j = 0; j < EVENTNUMBER; j++)
		{
			CString s;
			s.Format("#%d\n", j);
			file.WriteString(s);
			for (int k = 0; k < MAJOR6; k++)
				file.WriteString(m_strTextMatrix[j][k] + "\n");
		}
	}
	file.Close();
*/
}

/// Funktion gibt einen bestimmten Moralwert zur�ck.
/// @param byMappedRaceNumber gemappte Rassennummer
/// @param Event Eventnummer
/// @return Moralwert
short CMoralObserver::GetMoralValue(BYTE byMappedRaceNumber, unsigned short Event)
{
	// derzeit nur 6 Moralwertdatens�tze vorhanden
	ASSERT(byMappedRaceNumber >= 1 && byMappedRaceNumber <= 6);

	return m_iMoralMatrix[Event][byMappedRaceNumber - 1];
}
