#include "stdafx.h"
//#include "Constants.h"
#include "CombatShip.h"
#include "Ships.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CCombatShip::CCombatShip(void)
{
	m_pShip = NULL;
	m_pTarget = NULL;
	m_iModifier = 100;
	m_Fire.phaserIsShooting = FALSE;
	m_bRegShieldStatus = FALSE;
	m_byCloak = FALSE;
	m_byReCloak = FALSE;
	m_bShootCloaked = FALSE;
	m_byRetreatCounter = rand()%50 + 100;
	m_pKilledByShip = NULL;

	// Auswirkungen durch Anomalien
	m_bCanUseShields = TRUE;
	m_bCanUseTorpedos = TRUE;
	m_bFasterShieldRecharge = FALSE;
}

CCombatShip::~CCombatShip(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion gibt den m�glichen Bonus durch die Man�vriebarkeit bei einem Angriff zur�ck. �bergeben m�ssen
/// daf�r die Man�vrierbarkeit des Angreifers und die Man�vrierbarkeit des Verteidigers werden.
BYTE CCombatShip::GetToHitBoni(BYTE Att, BYTE Def)
{
/*
	Maneuvrierf�higkeit - Trefferbonus (= to hit bonus)
	---0---1---2---3---4---5---6---7---8---9
	0 00% 00% 00% 00% 00% 00% 00% 00% 00% 00%
	1 05% 00% 00% 00% 00% 00% 00% 00% 00% 00%
	2 10% 05% 00% 00% 00% 00% 00% 00% 00% 00%
	3 15% 10% 05% 00% 00% 00% 00% 00% 00% 00%
	4 20% 15% 10% 05% 00% 00% 00% 00% 00% 00%
	5 25% 20% 15% 10% 05% 00% 00% 00% 00% 00%
	6 30% 25% 20% 15% 10% 05% 00% 00% 00% 00%
	7 35% 30% 25% 20% 15% 10% 05% 00% 00% 00%
	8 40% 35% 30% 25% 20% 15% 10% 05% 00% 00%
	9 45% 40% 35% 30% 25% 20% 15% 10% 05% 00%
*/
	BYTE boni[10][10] = {
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		5,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		10, 5,	0,	0,	0,	0,	0,	0,	0,	0,
		15, 10, 5,	0,	0,	0,	0,	0,	0,	0,
		20, 15, 10, 5,	0,	0,	0,	0,	0,	0,
		25, 20, 15, 10, 5,	0,	0,	0,	0,	0,
		30, 25, 20, 15, 10, 5,	0,	0,	0,	0,
		35, 30, 25, 20, 15, 10,	5,	0,	0,	0,
		40, 35, 30, 25, 20, 15,	10,	5,	0,	0,
		45, 40, 35, 30, 25, 20,	15,	10,	5,	0
	};
MYTRACE("shipdetails")(MT::LEVEL_DEBUG, "boni[Att][Def]: %i, %i\n",boni[Att], boni[Def]);
	return boni[Att][Def];
}

/// Diese Funktion gibt den m�glichen Malus durch die Man�vriebarkeit bei einem Angriff zur�ck. �bergeben m�ssen
/// daf�r die Man�vrierbarkeit des Angreifers und die Man�vrierbarkeit des Verteidigers werden.
BYTE CCombatShip::GetToHitMali(BYTE Att, BYTE Def)
{
/*
	Maneuvrierf�higkeit / Verteidigungsbonus in % (= to hit malus f�r Gegner)
	---0---1---2---3---4---5---6---7---8---9
	0 00% 00% 00% 00% 00% 00% 00% 00% 00% 00%
	1 04% 00% 00% 00% 00% 00% 00% 00% 00% 00%
	2 10% 04% 00% 00% 00% 00% 00% 00% 00% 00%
	3 18% 10% 04% 00% 00% 00% 00% 00% 00% 00%
	4 28% 18% 10% 04% 00% 00% 00% 00% 00% 00%
	5 40% 28% 18% 10% 04% 00% 00% 00% 00% 00%
	6 54% 40% 28% 18% 10% 04% 00% 00% 00% 00%
	7 70% 54% 40% 28% 18% 10% 04% 00% 00% 00%
	8 88% 70% 54% 40% 28% 18% 10% 04% 00% 00%
	9 96% 88% 70% 54% 40% 28% 18% 10% 04% 00%
*/
	BYTE mali[10][10] = {
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		4,	0,	0,	0,	0,	0,	0,	0,	0,	0,
		10, 4,	0,	0,	0,	0,	0,	0,	0,	0,
		18, 10, 4,	0,	0,	0,	0,	0,	0,	0,
		28, 18, 10, 4,	0,	0,	0,	0,	0,	0,
		40, 28, 18, 10, 4,	0,	0,	0,	0,	0,
		54, 40, 28, 18, 10, 4,	0,	0,	0,	0,
		70, 54, 40, 28, 18, 10,	4,	0,	0,	0,
		88, 70, 54, 40, 28, 18,	10,	4,	0,	0,
		96, 88, 70, 54, 40, 28,	18,	10,	4,	0
	};

	return mali[Def][Att];
}

// Diese Funktion berechnet den n�chsten Punkt auf dem Weg zum Ziel, den das Schiff erreichen wird.
void CCombatShip::CalculateNextPosition()
{
	// Firecounter werden hier heruntergez�hlt. Sp�ter sollten wir das erst in der Schadensberechnung machen
	for (int i = 0; i < m_Fire.phaser.GetSize(); i++)
		if (m_Fire.phaser[i] > 0)
			m_Fire.phaser[i]--;
	for (int i = 0; i < m_Fire.torpedo.GetSize(); i++)
		if (m_Fire.torpedo[i] > 0)
			m_Fire.torpedo[i]--;

	if (m_byCloak == 0 && m_byReCloak < 255 && m_pShip->GetCloak())
		m_byReCloak++;

	// Wenn das Schiff getarnt war und geschossen hat, dann den Tarncounter runterz�hlen
	if (m_bShootCloaked == TRUE && m_byCloak > 0)
		m_byCloak--;

	// Kann das Schiff nicht fliegen, so gibt es keine neue Position
	if (m_byManeuverability == 0)
		return;

	// Wenn noch keine Flugroute berechnet wurde
	if (m_lRoute.empty())
	{
		// Angreifen
		if (m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_ATTACK)
		{
			// Wenn noch kein Ziel aufgeschaltet wurde
			if (m_pTarget != NULL)
			{
				// Damit die Schiffe nicht immer auf einer Stelle h�ngen, wird bei zu kurzer Distanz
				// ein zuf�lliges neues Ziel verwendet (minimale Distanz ist 10)
				CalcRoute(m_pTarget->m_KO, 10);
			}

			return;
		}

		// Meiden
		if (m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_AVOID)
		{
			// zuf�llige Zielposition im Raum
			CalcRoute(m_KO, 100);
			return;
		}

		// R�ckzug
		if (m_pShip->GetCombatTactic() == COMBAT_TACTIC::CT_RETREAT)
		{
			// m�glichen fr�hesten R�ckzugszeitpunkt runterz�hlen
			if (m_byRetreatCounter > 0)
			{
				// Z�hler runterz�hlen
				m_byRetreatCounter--;
				// Wenn Z�hler gleich null ist ein zuf�lliges Ziel w�hlen
				if (m_byRetreatCounter == 0)
				{
					// zuf�llige Zielposition im Raum
					vec3i b;
					b.x = rand()%30; b.y = rand()%30; b.z = rand()%30;
					CalcRoute(b);
				}
			}
			return;
		}
	}
}

// Funktion berechnet die Route zum Ziel
void CCombatShip::CalcRoute(const vec3i& ptTarget, int nMinDistance/* = 0*/)
{
	// unsere aktuelle Position im Raum
	vec3i a = m_KO;
	// die Zielposition im Raum
	vec3i b = ptTarget;
	// der Abstand zwischen diesen beiden Punkten
	int distance = a.Distance(b);

	if (distance < nMinDistance)
	{
		short multi = rand()%2;
		if (multi == 0)
			multi = -1;
		b.x = rand()%200 * multi;
		b.y = rand()%200 * multi;
		b.z = rand()%200 * multi;
		distance = a.Distance(b);
	}

	// die Beschleunigung des Schiffes. Bei z.B. einer Man�vriebarkeit von 5 wird bei Speed 1 gestartet und dann
	// immer um 1 erh�ht, bis die Speed von 5 erreicht ist. Genauso muss dann auch wieder verlangsamt werden.
	BYTE speed_up = 0;
	BYTE speed = this->m_byManeuverability;
	while (distance >= 1)
	{
		// "Bremsweg" berechnen -> speed
		BYTE speed_up_way = 0;	// Bremsweg, den wir mindst. brauchen um anzuhalten
		for (int i = speed_up; i > 0; i--)
			speed_up_way += i;
		// m�ssen bremsen
		if (distance <= speed_up_way && speed_up > 0)
			speed_up--;
		// k�nnen beschleunigen
		else if (speed_up < speed)
			speed_up++;
		// Wenn unser Schiff weiterfliegen k�nnte als der Abstand zwischen den Schiffen, dann
		// m�ssen wir die Geschwindigkeit herabsetzen
		if (speed_up > distance)
			speed_up = (BYTE)distance;

		float multi = (float)speed_up / (float)distance;
		vec3i temp = b - a;
		// Runden durch floor und +0.5
		temp.x = (int)(floor)(temp.x * multi + 0.5); temp.y = (int)(floor)(temp.y * multi + 0.5); temp.z = (int)(floor)(temp.z * multi + 0.5);

		//a = a + (b-a) * (speed_up / distance);
		a = a + temp;

		m_lRoute.push_back(a);
		distance = a.Distance(b);

		//CString s;
		//s.Format("Ship: %s\nSpeedUp: %d\nold Pos: %d, %d, %d\nnew Pos: %d, %d, %d\nenemy Pos: %d, %d, %d\nDistance: %d", m_pShip->GetShipClass(), speed_up, m_KO.x, m_KO.y, m_KO.z, a.x, a.y, a.z, b.x, b.y, b.z, distance);
		//AfxMessageBox(s);
	}
}

// Diese Funktion setzt das Schiff auf den n�chsten Punkt seiner vorher berechneten Flugroute.
void CCombatShip::GotoNextPosition()
{
	// Wenn wir schon eine Route berechnet haben, dann nur auf den n�chsten Punkt in der Route gehen
	if (!m_lRoute.empty())
	{
		m_KO = m_lRoute.front();
		// Routeneintrag entfernen
		m_lRoute.pop_front();
	}
}

// Diese Funktion f�hrt einen Beamangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
// gespeichert ist
CPoint CCombatShip::AttackEnemyWithBeam(const CPoint& beamStart)
{
	// Wenn wir ein Ziel aufgeschaltet haben
	if (m_pTarget != NULL)
	{
		m_Fire.phaserIsShooting = FALSE;
		// Nur wenn die Distanz gr��er als ein Feld ist wird geschossen, sonst ja gerammt
		int distance = m_KO.Distance(m_pTarget->m_KO);
		if (distance >= 1)
		{
			// Bonus durch Schiffsspezialeigenschaften besorgen
			BYTE boni = GetAccBoniFromSpecials();
			int nBeamWeapon = beamStart.x;
			int nBeamRay	= beamStart.y;
			// alle Beamwaffen durchgehen
			for (int i = nBeamWeapon; i < m_pShip->GetBeamWeapons()->GetSize(); i++)
			{
				if (nBeamRay >= m_pShip->GetBeamWeapons()->GetAt(i).GetBeamNumber())
				{
					nBeamRay = 0;
					continue;
				}

				// Solange der Counter gr��er als die Rechargetime ist wird geschossen. Dann wird diese runtergez�hlt.
				if (m_Fire.phaser[i] > m_pShip->GetBeamWeapons()->GetAt(i).GetRechargeTime() || m_Fire.phaser[i] == NULL)
				{
					// Reichweite = Type(MK)*5 + (Bonus durch Feuer-System) + 50 (um auf 60 bis 160 zu kommen)
					if (distance <= (m_pShip->GetBeamWeapons()->GetAt(i).GetBeamType()*5 + m_pShip->GetBeamWeapons()->GetAt(i).GetBonus() + 50))
					{
						if (AllowFire(m_pShip->GetBeamWeapons()->GetAt(i).GetFirearc()))
						{
							// Hier wird der Phasercounter auf die Rechargetime + die Beamschussdauer festgelegt.
							// Solange also dieser Counter gr��er als die Rechargetime ist wird geschossen. Dann wird gewartet
							// bis die Rechargetime auch verstrichen ist. Erst dann erfolgt wieder diese Zuweisung.
							if (m_Fire.phaser[i] == NULL)
								m_Fire.phaser[i] = m_pShip->GetBeamWeapons()->GetAt(i).GetRechargeTime() + m_pShip->GetBeamWeapons()->GetAt(i).GetBeamLenght();
							// Hier den Schaden f�r jeden Beamstrahl einzeln berechnen. Also die Schleife gleich der Anzahl der
							// Beamstrahlen der Beamwaffe durchlaufen
							while (nBeamRay++ < m_pShip->GetBeamWeapons()->GetAt(i).GetBeamNumber())
							{
								m_Fire.phaserIsShooting = FALSE;

								// Nur f�r die Darstellung im CombatSimulator
								// ------------------------------------------
								m_bPulseFire = FALSE;
								if (m_pShip->GetBeamWeapons()->GetAt(i).GetShootNumber() > 1)
									m_bPulseFire = TRUE;
								// ------------------------------------------
								// Ende der besonderen Darstellung

								// Angriff und Schadensberechnung
								FireBeam(i, distance, boni);

								// Wenn das Ziel keine H�lle mehr hat, dann geben wir die aktuelle Nummer des Beams zur�ck.
								// Dies bedeutet, das wir kein Ziel mehr haben und uns ein neues suchen m�ssen. Bei der aktuellen
								// Nummer des Beams geht dann die Berechnung weiter
								if (m_pTarget->m_pShip->GetHull()->GetCurrentHull() == 0)
									return CPoint(i, nBeamRay);
							}
							// bei der n�chsten Waffe kann wieder mit dem ersten Strahl geschossen werden
							nBeamRay = 0;
						}
					}
				}
			}
		}
	}
	return CPoint(-1,-1);
}

// Diese Funktion f�hrt einen Torpedoangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget</code>
// gespeichert ist
CPoint CCombatShip::AttackEnemyWithTorpedo(std::list<CTorpedo*>* pCT, const CPoint& torpedoStart)
{
	// Wenn wir ein Ziel aufgeschaltet haben
	if (m_pTarget != NULL)
	{
		// Nur wenn die Distanz gr��er als drei Felder ist wird geschossen, sonst ja gerammt
		int distance = m_KO.Distance(m_pTarget->m_KO);
		// Wenn das Schiff getarnt ist, dann wird fr�hestens gefeuert, wenn man auf eine Distanz von 2/3 der maximalen
		// Torpedoreichweite rangeflogen ist.
		if (m_bShootCloaked == FALSE && distance > MAX_TORPEDO_RANGE / 1.5)
			return CPoint(-1,-1);

		if (distance >= 3)
		{
			// Bonus durch Schiffsspezialeigenschaften besorgen
			BYTE boni = GetAccBoniFromSpecials();

			// voraussichtlicher Gesamtschaden dieser Torpedoattacke
			//UINT damage = 0;
			// Anzahl der verschieden Launcher durchgehen
			for (int i = torpedoStart.x; i < m_pShip->GetTorpedoWeapons()->GetSize(); i++)
			{
				if (m_Fire.torpedo[i] == 0 || torpedoStart.y > 0)
				{
					// Jetzt das Ziel des Torpedos berechnen. Daf�r den Kurs des Zielschiffes anschauen. Berechnen
					// nach welcher Zeit der Torpedo sich mit dem Ziel treffen w�rde. Dort hin m�ssen wir schie�en.
					vec3i targetKO = m_pTarget->m_KO;
					if (distance > TORPEDOSPEED)
					{
						int oldDist = distance;
						// Auf den wievielten Routeneintrag des Schiffes wird gerade gezielt?
						USHORT route = 0;
						do {
							// Hat unser Ziel eine Route gespeichert?
							if (m_pTarget->m_lRoute.size() > route)
							{
								distance = m_KO.Distance(m_pTarget->m_lRoute.at(route));
								targetKO = m_pTarget->m_lRoute.at(route);
								route++;
							}
							else
								break;
						} while ((distance / TORPEDOSPEED) >= route);
						distance = oldDist;
					}

					// Wenn die Distanz nicht gr��er als 200 ist werden die Torpedos abgefeuert
					// Hier �berpr�fe ich mit der old_distance. Weil wenn ich die nehmen w�rde, die wir nach einer
					// bestimmten Zeit erst haben, sind die Schiffe, die schnell fliegen im Nachteil. Deren Gegner
					// w�rden zuerst schie�en, weil die Distanz schneller kleiner wird.
					if (distance <= MAX_TORPEDO_RANGE)
					{
						if (AllowFire(m_pShip->GetTorpedoWeapons()->GetAt(i).GetFirearc()))
						{
							// Anzahl dieses bestimmten Launchers durchgehen
							int n = torpedoStart.y;
							while (n++ < m_pShip->GetTorpedoWeapons()->GetAt(i).GetNumberOfTupes())
							{
								// Wenn der voraussichtliche Torpedogesamtschaden viel gr��er ist als die noch aktuelle H�lle
								// plus Schilde des Gegners, dann Abbruch und neues Ziel aufschalten und angreifen. Wir geben
								// hier die Art und die aktuelle Numer des Beams zur�ck
								//if (damage > ((m_pTarget->m_pShip->GetShield()->GetCurrentShield()
								//	+ m_pTarget->m_pShip->GetHull()->GetCurrentHull()) * 1.5))
								//	return CPoint(i,n);
								/*damage +=*/ FireTorpedo(pCT, i, targetKO, boni);
							}

							//CString s;
							//s.Format("Feuert Torpedo: %s auf %s", m_pShip->GetShipClass(), m_pTarget->m_pShip->GetShipClass());
							//MYTRACE("general")(MT::LEVEL_INFO, s);

						}
					}
				}
			}
		}
	}
	return CPoint(-1,-1);
}

// Diese Funktion gibt den Modifikator, den wir durch die Crewerfahrung erhalten zur�ck
BYTE CCombatShip::GetCrewExperienceModi()
{
	/* Jedes Erfahrungslevel gibt einen Bonus von 20% auf den Schaden. Au�erdem ist die Trefferwahrscheinlichkeit h�her.
	CString exp;
	exp.Format("Bonus auf Grund der Schiffserfahrung: %d%%\nLevel: %d", m_pShip->GetExpLevel() * 20, m_pShip->GetExpLevel());
MYTRACE("general")(MT::LEVEL_INFO, exp);
	//AfxMessageBox(exp);
	//
*/
	if (m_pShip != NULL)
		return m_pShip->GetExpLevel() * 20;
	else
		return 0;
}

/// Diese Funktion stellt die vorhandenen regenerativen Schilde auf die feindliche schilddurchschlagende Waffe ein.
void CCombatShip::ActRegShield(void)
{
	if (m_pShip->GetShield()->GetRegenerative())
		m_bRegShieldStatus = TRUE;
}

/// Diese Funktion berechnet den Angriffsbonus, den Schiffe auf Grund ihrer Spezialeigenschaften wom�glich erhalten.
BYTE CCombatShip::GetAccBoniFromSpecials(void)
{
	int boni = 0;

	// Wenn getarnt geschossen wurde, so gibt es einen 20% Trefferbonus und Schadensbonus
	if (m_byCloak > 0)
		boni += 20;
	// Wenn wir die Dogfighter/Nahk�mpfer Eigenschaft haben, dann bekommen wir einen 20% Bonus, wenn
	// wir gegen kleine Gegner k�mpfen
	if (m_pShip->HasSpecial(SHIP_SPECIAL::DOGFIGHTER) && m_pTarget->m_pShip->GetShipSize() == SHIP_SIZE::SMALL)
		boni += 30;
	// Wenn wir die Dogkiller/Antinahk�mpfer Eigenschaft haben, dann bekommen wir einen 20% Bonus gegen Dogfighter/Nahk�mpfer
	if (m_pShip->HasSpecial(SHIP_SPECIAL::DOGKILLER) && m_pTarget->m_pShip->HasSpecial(SHIP_SPECIAL::DOGFIGHTER))
		boni += 30;

	ASSERT(boni >= 0 && boni <= 255);
	return (BYTE)boni;
}

// Funktion berechnet, ob ein Feuersystem aufgrund der Position des Schiffes, der Position des Systems auf dem Schiff und
// dessen Feuerwinkel auch feuern kann.
// @param arc Zeiger auf Schussfeld
// @return Wahrheitswert
bool CCombatShip::AllowFire(const CFireArc* arc)
{
	if (m_pShip->IsStation())
		return true;
	/*
	Ich fasse nochmal zusammen was ben�tigt wird und was mittels Variablen festgehalten werden m�sste:

	- Ausrichtung des Schiffes (kann schnell berechnet werden)
	- Feuerwinkel jeder einzelnen Waffe (m�sste noch als Attribut hinzugef�gt werden -> �nderung des ShipEditors und der data Datei,
	  horizontaler == vertikaler Winkel?)
	- Position des gegnerischen Schiffes (ist vorhanden)
	-> aus diesen Variablen muss ein boolescher Wert berechnet werden, ob ein Feuern gestattet ist
	*/
	int dirarc_V = arc->GetPosition();
	int firearc_V = arc->GetAngle();
	bool allow = false;
    //int m=2; //"Regelkonstante"

	vec3<float> v;
	// Besitzt das Schiff eine Flugroute, so ist dessen Ausrichtung der Vektor zwischen aktueller Position und dem
	// n�chsten Punkt auf der Flugroute
	if (!m_lRoute.empty())
	{
		vec3i p = m_lRoute[0] - m_KO;
		v.Set((float)p.x, (float)p.y, (float)p.z);
		v /= v.Length();
	}
	// Ohne Flugziel richtet sich das Schiff automatisch zum Zentrum aus P(0,0,0)
	else
	{
		v.Set((float)-m_KO.x, (float)-m_KO.y, (float)-m_KO.z);
		v /= v.Length();
	}

	// Vektor zwischen Schiff und Ziel
	vec3<float> diff;
	if (m_pTarget != 0)
	{
		vec3i p = m_pTarget->m_KO - m_KO;
		diff.Set((float)p.x, (float)p.y, (float)p.z);
		diff /= diff.Length();
	}

	int diffarc = 0;
	// new_z =(v)x(diff)=(v2*diff3 - v3*diff2, v3*diff1 - v1*diff3, v1*diff2 - v2*diff1);
	vec3<float> new_z = v / diff; // Kreuzprodukt geht auch mit v ^ diff und ^-Operator
	// If (new_z=0)    #Der Nullvektor, d.h. v=k*diff (k w�re ein Skalar)
	if (new_z.IsNull())
	{
	  // If Skalar(v,diff)>=0 then diffarc=0 else diffarc=180; #ginge auch �ber norm(diff+v)>norm(diff)
	  if (v * diff >= 0)
		diffarc = 0;
	  else
		diffarc = 180;
	}
	else
	{
		//Regeln f�r Verhindern der positiven Achsenausrichtung und damit immer gleiche Seite beim Feuern
		vec3<float> g_x;
		vec3<float> g_z;
		vec3<float> g_y(diff / diff.Length()); //unser g_y bleibt immer gleich, man h�tte auch g_y ver�ndern k�nnen, ich hab mich f�r g_x entschieden
		vec3<float> gYTemp((v * g_y) * g_y); //damit bleibt auch gYTemp immer gleich

		/*if (firepower_centroid > m)
		{
			g_x = ((gYTemp - v) / (gYTemp - v).Length());
			g_z = ((-new_z) / new_z.Length());
		}
		else if (firepower_centroid < m)
		{
			g_x = ((v - gYTemp) / (v - gYTemp).Length());
			g_z((new_z) new_z.Length());
		}
		else*/
		if (diff.y < 0)
		{
			if (diff * v < 0)
			{
				g_x = ((gYTemp - v) / (gYTemp - v).Length());
				g_z = ((-new_z) / new_z.Length());
			}
			else
			{
				g_x = ((v - gYTemp) / (v - gYTemp).Length());
				g_z = (new_z / new_z.Length());
			}
		}
		else if (diff.y > 0)
		{
			if (diff * v < 0)
			{
				g_x = ((v - gYTemp) / (v - gYTemp).Length());
				g_z = (new_z / new_z.Length());
			}
			else
			{
				g_x = ((gYTemp - v) / (gYTemp - v).Length());
				g_z = ((-new_z) / new_z.Length());
			}
		}
		else if (diff.x < 0)
		{
			 if (diff * v < 0)
			 {
				 g_x = ((gYTemp - v) / (gYTemp - v).Length());
				 g_z = ((-new_z) / new_z.Length());
			 }
			 else
			 {
				 g_x = ((v - gYTemp) / (v - gYTemp).Length());
				 g_z = (new_z / new_z.Length());
			 }
		}
		else if (diff.x > 0)
		{
			 if (diff * v < 0)
			 {
				 g_x = ((v - gYTemp) / (v - gYTemp).Length());
				 g_z = (new_z / new_z.Length());
			 }
			 else
			 {
				 g_x = ((gYTemp - v) / (gYTemp - v).Length());
				 g_z = ((-new_z) / new_z.Length());
			 }
		}
		else if (diff.z < 0)
		{
			 if (diff * v < 0)
			 {
				 g_x = ((gYTemp - v) / (gYTemp -v).Length());
				 g_z = ((-new_z) / new_z.Length());
			 }
			 else
			 {
				 g_x = ((v - gYTemp) / (v - gYTemp).Length());
				 g_z = (new_z / new_z.Length());
			 }
		}
		else if (diff.z > 0)
		{
			 if (diff * v < 0)
			 {
				 g_x = ((v - gYTemp) / (v - gYTemp).Length());
				 g_z = (new_z / new_z.Length());
			 }
			 else
			 {
				 g_x = ((gYTemp - v) / (gYTemp - v).Length());
				 g_z = ((-new_z) / new_z.Length());
			 }
		}
		else
		{
			g_x = ((v - gYTemp) / (v - gYTemp).Length());
			g_z = ((new_z) / new_z.Length());
		}

		// v=Matrixprodukt((a1,a2,a3;b1,b2,b3;c1,c2,c3),(v1,v2,v3))
		vec3<float> V(g_x.x * v.x + g_x.y * v.y + g_x.z * v.z, g_y.x * v.x + g_y.y * v.y + g_y.z * v.z, g_z.x * v.x + g_z.y * v.y + g_z.z * v.z);
		// diff=Matrixprodukt((a1,a2,a3;b1,b2,b3;c1,c2,c3),(diff1,diff2,diff3))
		vec3<float> DIFF(g_x.x * diff.x + g_x.y * diff.y + g_x.z * diff.z, g_y.x * diff.x + g_y.y * diff.y + g_y.z * diff.z, g_z.x * diff.x + g_z.y * diff.y + g_z.z * diff.z);

		// diffv_arcdiff=(Skalarprodukt(diff_new,v_new),diff_new2*v_new1 - diff_new1*v_new2) #als komplex-konjugiert-multiplizierte komplexe Zahl und die wiederum als 2D-Vektor dargestellt. Der Fall    dass diffv_arcdiff1 und diffv_arcdiff2 0 sind, also der gesamte Vektor der Nullvektor darstellt, kann nicht auftreten, den haben wir mit der If-Abfrage oben schon rausgenommen.
		vec3<float> diffv_arcdiff(DIFF * V / V.Length(), DIFF.y * V.x / V.Length(), 0); //DIFF.x = 0 ! d.h. es steht eigentlich nur DIFF.y*V.X und das ist 1*1=1
		// diffarc_bogenma�=arg(z)={arctan(y/x) f�r x<0, Pi/2 f�r x=0 und y>0, Pi + arctan(y/x) f�r x>0, 3/2 Pi f�r x=0, y<0} oder auch diffarc_bogenma�=arg(z)={arccos(a/r) f�r y>=0, 2pi - arccos(a/r) f�r y<0}
		float diffarc_bogenmass;
		if (diffv_arcdiff.y < 0) //zweite Komponente kleiner 0, d.h. Vektor in unterer Halbebene und damit das Feindschiff links vom eigenen Schiff aus gesehen
			diffarc_bogenmass = (float)PI * 2 - acos((float)diffv_arcdiff.x / (float)diffv_arcdiff.Length());
		else
			diffarc_bogenmass = acos((float)diffv_arcdiff.x / (float)diffv_arcdiff.Length());
		//diffarc=diffarc_bogenma�*180/Pi
		diffarc = (int)(diffarc_bogenmass * 180 / (float)PI);
	}

	if ( (dirarc_V - firearc_V/2 < diffarc - 360) || ((dirarc_V - firearc_V/2 < diffarc) && (diffarc < dirarc_V + firearc_V/2))
	  || (diffarc + 360 < dirarc_V + firearc_V/2) )
	  allow = true;

	return allow;
}

// Diese Funktion berechnet die Trefferwahrscheinlichkeit des Beams und f�gt dem Feindschiff wenn m�glich Schaden zu.
// @param beamWeapon Nummer der Beamwaffe
// @param distance Distanz zwischen unserem Schiff und dem Gegner
// @param boni Bonus durch Schiffseigenschaften
void CCombatShip::FireBeam(int beamWeapon, int distance, BYTE boni)
{
	// DAMAGE_TO_HULL (10%) des Schadens gehen immer auf die H�lle.
	int beamDamage = 0;
	int toHull = 0;

	// Es wird Anzahl der Beams pro Schuss beachtet, meist sind das nur einer.
	// Bei Cannon- und Pulsewaffen k�nnen es aber mehrere sein, die pro Schuss/Sekunde rauskommen
	for (int t = 0; t < m_pShip->GetBeamWeapons()->GetAt(beamWeapon).GetShootNumber(); t++)
	{
		// Hier kommt die Formel zur Trefferwahrscheinlichkeit ins Spiel. Jeder Schuss wird daf�r einzeln beachtet:
		// Acc = 60 - Distance*0.1 + CrewExp(Attacker-Target)*0.1 + Man�vriebarkeit(Attacker-Target)*0.2 + Bonus_durch_Feuersystem + BeamType(MK)
		// distance = [0,16]; CrewExp(att-def) = [-12,12]; Man�ver(att-def) = [-15,6]; Feuersystembonus = [5,30]; BeamtypeMK = [1,X] X max. ca. 20
		short Acc = (short)(60 - distance * 0.1 + (GetCrewExperienceModi() - m_pTarget->GetCrewExperienceModi()) * 0.1
			+ (GetToHitBoni(m_pShip->GetManeuverability(), m_pTarget->m_pShip->GetManeuverability()) - GetToHitMali(m_pShip->GetManeuverability(), m_pTarget->m_pShip->GetManeuverability())) * 0.2
			+ m_pShip->GetBeamWeapons()->GetAt(beamWeapon).GetBonus() + m_pShip->GetBeamWeapons()->GetAt(beamWeapon).GetBeamType());

		// Modifikation durch die Schiffsgr��e
		if (m_pTarget->m_pShip->GetShipSize() == SHIP_SIZE::SMALL)
			Acc = (short)(Acc * 0.66);
		else if (m_pTarget->m_pShip->GetShipSize() == SHIP_SIZE::BIG)
			Acc = (short)(Acc * 1.33);
		else if (m_pTarget->m_pShip->GetShipSize() >= SHIP_SIZE::HUGE)
			Acc = (short)(Acc * 1.66);

		// Wird Schaden gemacht?
		if ((Acc + boni) > rand()%100)
			beamDamage += ((m_pShip->GetBeamWeapons()->GetAt(beamWeapon).GetBeamPower() * (m_iModifier + boni)) / m_pTarget->m_iModifier);

		//CString s;
		//s.Format("Our Man: %d\nTo Hit Boni: %d\nTo Hit Mali: %d",this->m_byManeuverability,GetToHitBoni(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability()),GetToHitMali(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability()));
		//s.Format("Acc: %d\ndistance: %d\nCrewmod: %d\nMan�verMod: %d\nSystemboni: %d\nMK_Boni: %d", Acc,(short)(-distance*0.1),(short)((this->GetCrewExperienceModi()-this->m_pTarget->GetCrewExperienceModi())*0.1),short((GetToHitBoni(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability())-GetToHitMali(m_pShip->GetManeuverability(),m_pTarget->m_pShip->GetManeuverability()))*0.2),m_pShip->GetBeamWeapons()->GetAt(i).GetBonus(),m_pShip->GetBeamWeapons()->GetAt(i).GetBeamType());
		//AfxMessageBox(s);
	}

	// Sobald ein getarntes Schiff mit einem Beam getroffen hat wird festgelegt, das es geschossen hat
	if (beamDamage > 0)
		m_bShootCloaked = TRUE;

	// Wenn das feindliche Schiff keine ablative H�llenpanzerung hat, dann gehen 10% des Schadens sofort
	// auf die H�lle
		if (m_pTarget->m_pShip->GetHull()->GetAblative() == FALSE)
		toHull = (int)(beamDamage * DAMAGE_TO_HULL);
	// wenn wir schilddurchdringende Beams haben und das feindliche Schiff keine auf unsere Beams eingestellten
	// regenerativen Schilde hat, dann machen wir kompletten Schaden an der H�lle
	if (m_pShip->GetBeamWeapons()->GetAt(beamWeapon).GetPiercing() == TRUE && m_pTarget->GetActRegShields() == FALSE)
		toHull = beamDamage;
	// wenn wir modulierende Beams haben, dann gehen immer 50% auf die H�lle und 50% auf die Schilde, egal
	// was f�r Schilde an dem Zielschiff sind
	else if (m_pShip->GetBeamWeapons()->GetAt(beamWeapon).GetModulating() == TRUE)
		toHull = (int)(beamDamage * 0.5);
	m_pTarget->m_pShip->GetHull()->SetCurrentHull(-toHull);

	//CString dam;
	//dam.Format("Schiff: %s\naltes Schild: %d\nH�lle: %d\nBeamschaden: %d\nBeamschaden auf H�lle: %d",	m_pTarget->m_pShip->GetShipClass(), m_pTarget->m_pShip->GetShield()->GetCurrentShield(), m_pTarget->m_pShip->GetHull()->GetCurrentHull(), beamDamage, toHull);
	//AfxMessageBox(dam);

	// der restliche Beamschaden ermitteln, welcher nicht direkt auf die H�lle ging
	beamDamage -= toHull;
	MYTRACE("combatdetails")(MT::LEVEL_DEBUG, "CombatShip.cpp: Shipclass: %s, Shipname: %s, beamDamage= %i\n", m_pTarget->m_pShip->GetShipClass(), m_pTarget->m_pShip->GetShipName(), beamDamage);
	// Der komplette Schaden geht auf die Schilde (wenn diese verwendet werden k�nnen)
	if ((int)m_pTarget->m_pShip->GetShield()->GetCurrentShield() - beamDamage >= 0 && m_bCanUseShields)
		m_pTarget->m_pShip->GetShield()->SetCurrentShield((int)m_pTarget->m_pShip->GetShield()->GetCurrentShield()-beamDamage);
	// Der Schaden geht teilweise auf Schilde und H�lle oder nur auf die H�lle
	else
	{
		m_pTarget->m_pShip->GetHull()->SetCurrentHull((int)m_pTarget->m_pShip->GetShield()->GetCurrentShield() - beamDamage);
		m_pTarget->m_pShip->GetShield()->SetCurrentShield(0);
	}

	// Wenn wir schilddurchdringende Beams haben und das feindliche Schiff regenerative Schilde, so kann es diese
	// auf unsere Waffen einstellen
	if (m_pShip->GetBeamWeapons()->GetAt(beamWeapon).GetPiercing() == TRUE && m_pTarget->m_pShip->GetShield()->GetRegenerative() == TRUE)
		m_pTarget->ActRegShield();
	m_Fire.phaserIsShooting = TRUE;

	//CString dam;
	//dam.Format("Beamschaden auf Schiff: %s neues Schild: %d H�lle: %d restlicher Beamschaden: %d",	m_pTarget->m_pShip->GetShipClass(), m_pTarget->m_pShip->GetShield()->GetCurrentShield(), m_pTarget->m_pShip->GetHull()->GetCurrentHull(), beamDamage);
	//AfxMessageBox(dam);

	// Merken welche Rasse das Schiff schlussendlich zerst�rt hat
	if (!m_pTarget->m_pShip->IsAlive())
		m_pTarget->m_pKilledByShip = m_pShip;
}

// Diese Funktion feuert einen Torpedo auf das feindliche Schiff.
// @param CT Feld der Torpedos im Kampf
// @param beamWeapon Nummer der Beamwaffe
// @param targetKO Zielkoordinate des Torpedos
// @param boni Bonus durch Schiffseigenschaften
// @return maximal zu erwartender Schaden
UINT CCombatShip::FireTorpedo(std::list<CTorpedo*>* pCT, int torpedoWeapon, const vec3i& targetKO, BYTE boni)
{
	m_bShootCloaked = TRUE;
	m_Fire.torpedo[torpedoWeapon] = m_pShip->GetTorpedoWeapons()->GetAt(torpedoWeapon).GetTupeFirerate();
	// Soviele Torpedos dem Feld zuf�gen wie wir mit einem Mal mit dem Launcher verschie�en
	CTorpedo *torpedo = new CTorpedo();;
	torpedo->m_TargetKO = targetKO;
	torpedo->m_iNumber = m_pShip->GetTorpedoWeapons()->GetAt(torpedoWeapon).GetNumber();
	torpedo->m_KO = m_KO;
	torpedo->m_iPower = (m_pShip->GetTorpedoWeapons()->GetAt(torpedoWeapon).GetTorpedoPower() * (m_iModifier + boni) / 100);
	torpedo->m_byType = m_pShip->GetTorpedoWeapons()->GetAt(torpedoWeapon).GetTorpedoType();
	torpedo->m_pShipFiredTorpedo = m_pShip;
	MYTRACE("combatdetails")(MT::LEVEL_DEBUG, "CombatShip.cpp: m_pShipFiredTorpedo= %s\n", torpedo->m_pShipFiredTorpedo);
	torpedo->m_byManeuverability = this->m_byManeuverability;
	torpedo->m_iModi = GetCrewExperienceModi() + m_pShip->GetTorpedoWeapons()->GetAt(torpedoWeapon).GetAccuracy() + boni;
	// Torpedo dem Torpdofeld hinzuf�gen
	pCT->push_back(torpedo);

	return torpedo->m_iPower * torpedo->m_iNumber;
}
