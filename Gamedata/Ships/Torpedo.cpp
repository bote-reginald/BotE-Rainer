#include "stdafx.h"
#include "Torpedo.h"
#include "CombatShip.h"
#include "Ships.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTorpedo::CTorpedo(void) :
	m_iNumber(0),
	m_iPower(0),
	m_byType(0),
	m_byManeuverability(0),
	m_iModi(0),
	m_pShipFiredTorpedo(NULL)
{
	m_iDistance = 0;
}

CTorpedo::~CTorpedo(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CTorpedo::CTorpedo(const CTorpedo &rhs)
{
	m_KO.x  = rhs.m_KO.x;
	m_KO.y  = rhs.m_KO.y;
	m_KO.z  = rhs.m_KO.z;
	m_TargetKO.x  = rhs.m_TargetKO.x;
	m_TargetKO.y  = rhs.m_TargetKO.y;
	m_TargetKO.z  = rhs.m_TargetKO.z;
	m_iNumber = rhs.m_iNumber;
	m_iDistance = rhs.m_iDistance;
	m_iPower = rhs.m_iPower;
	m_byType = rhs.m_byType;
	m_pShipFiredTorpedo = rhs.m_pShipFiredTorpedo;
	m_iModi = rhs.m_iModi;
	m_byManeuverability = rhs.m_byManeuverability;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CTorpedo & CTorpedo::operator=(const CTorpedo & rhs)
{
	if (this == &rhs)
		return *this;
	m_KO.x  = rhs.m_KO.x;
	m_KO.y  = rhs.m_KO.y;
	m_KO.z  = rhs.m_KO.z;
	m_TargetKO.x  = rhs.m_TargetKO.x;
	m_TargetKO.y  = rhs.m_TargetKO.y;
	m_TargetKO.z  = rhs.m_TargetKO.z;
	m_iNumber = rhs.m_iNumber;
	m_iDistance = rhs.m_iDistance;
	m_iPower = rhs.m_iPower;
	m_byType = rhs.m_byType;
	m_pShipFiredTorpedo = rhs.m_pShipFiredTorpedo;
	m_iModi = rhs.m_iModi;
	m_byManeuverability = rhs.m_byManeuverability;
	return *this;
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

// Funktion berechnet die Flugbahn des Torpedos und macht die Kollisionsabfrage mit allen anderen Schiffen
// im Kampf. Wenn der Torpedo trifft, dann wird auch der Schaden am Schiff verrechnet und die Funktion gibt
// TRUE zur�ck. Wenn wir keinen Treffer landen, aber MAX_TORPEDO_RANGE �berschritten haben gibt die Funktion
// auch ein TRUE zur�ck. Ansonsten gibt die Funktion immer FALSE zur�ck
BOOLEAN CTorpedo::Fly(CArray<CCombatShip*,CCombatShip*>* CS)
{
	// Wir haben ja den St�tzpunkt unseres Torpedos und den Vektor zum Zielpunkt.
	// Jetzt �berpr�fe ich, ob ein feindliches Schiff auf diesem Vektor liegt dessen
	// Distanz kleiner als der TORPEDOSPPED (20) ist. Das Schiff mit der kleinsten Distanz
	// wird dann getroffen. Wenn ich kein Schiff finde, welches auf der Bahn liegt und
	// dessen Distanz kleiner TORPEDOSPEED ist oder ich finde gar kein Schiff auf der Bahn,
	// dann k�nnen wir auf dem Punkt nach TORPEDOSPEED-Entfernung mit dem Torpedo vorr�cken

	// St�tzpunkt ist der Punkt m_KO
	// Der Vektor ist der Vektor von m_KO zu m_TargetKO
	// t.x = (D.x - A.x) / C.x mit C = (B-A) wobei A = m_KO und B = m_TargetKO und D = CS->m_KO

	vec3i c = m_TargetKO - m_KO;
	int	minDistance = -1;
	short shipNumber = -1;
	for (int i = 0; i < CS->GetSize(); i++)
	{
		if (m_pShipFiredTorpedo->GetOwnerOfShip() != CS->GetAt(i)->m_pShip->GetOwnerOfShip() && CS->GetAt(i)->m_pShip->IsAlive())
		{
			vec3i t;
			if (c.x != 0)
				t.x = (CS->GetAt(i)->m_KO.x - m_KO.x) / c.x;
			else
				t.x = 0;
			if (c.y != 0)
				t.y = (CS->GetAt(i)->m_KO.y - m_KO.y) / c.y;
			else
				t.y = 0;
			if (c.z != 0)
				t.z = (CS->GetAt(i)->m_KO.z - m_KO.z) / c.z;
			else
				t.z = 0;
			// Wenn irgendein t ungleich null ist aber negativ ist, dann liegt das Schiff zwar auf unserer Flugbahn,
			// aber entgegengesetzt zur Torpedoflugrichtung. Also �berpr�fe ich das jedes t gr��er gleich NULL sein mu�
			if (t.x >= 0 && t.y >= 0 && t.z >= 0)
				if ((t.x == 0 && t.y == t.z) || (t.y == 0 && t.x == t.z) || (t.z == 0 && t.x == t.y) || (t.x == t.y == t.z))
					// Dann liegt das Schiff auf unserer Flugbahn, Distanz berechnen
					if (minDistance == -1 || minDistance > m_KO.Distance(CS->GetAt(i)->m_KO))
					{
						minDistance = m_KO.Distance(CS->GetAt(i)->m_KO);
						shipNumber = i;
					}
		}
	}

	// Wenn jetzt die minimale Distanz kleiner gleich der TORPEDOSPEED ist, dann gibts nen Einschlag
	if (minDistance <= TORPEDOSPEED && shipNumber != -1)
	{
		// Hier wird vielleicht Schaden am gegnerischen Schiff gemacht
		// Die Anzahl der auf diesem Feld gleichzeitigen Torpedos mit selben Ziel durchgehen
		BYTE count = 0;
		for (int i = 0; i < m_iNumber; i++)
			// Wenn der Torpedo trifft wird count um eins inkrementiert
			count += PerhapsImpact(CS->GetAt(shipNumber), (USHORT)minDistance);
		// Die Anzahl dieser Torpedos in dem Feld um die Anzahl der Torpedos verringern, welche ihr Ziel getroffen haben
		m_iNumber -= count;
		// Wenn m_iNumber gleich NULL ist, dann gibt es hier keine Torpedos mehr und wir k�nnen ein TRUE zur�ckgeben.
		// Dieses TRUE veranla�t, dass dieses Torpedoobjekt zerst�rt wird
		if (m_iNumber == NULL)
			return TRUE;
		// Die restlichen Torpedos fliegen ganz normal auf ihrer Bahn weiter, so als h�tten sie kein Ziel auf dieser
		// Bahn gehabt
		else
			return FlyToNextPosition();
	}
	// Wenn kein Schiff in Trefferreichweite ist k�nnen wir den Torpedo um TORPEDOSPEED weiterfliegen lassen
	else
		return FlyToNextPosition();
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

// Diese private Funktion berechnet den Schaden, den ein Torpedo am gegnerischen Schiff anrichtet.
void CTorpedo::MakeDamage(CCombatShip* CS)
{
	// DAMAGE_TO_HULL (10%) des Schadens gehen immer auf die H�lle.
	int torpedoDamage = (m_iPower * 100) / CS->m_iModifier;

/*	CString dam;
	dam.Format("Torpedoschaden an Schiff # %s (%s) # zuvor: Schilde: %d H�lle: %d Torpedoschaden: %d",
		CS->m_pShip->GetShipName(), CS->m_pShip->GetShipClass(), CS->m_pShip->GetShield()->GetCurrentShield(),
		CS->m_pShip->GetHull()->GetCurrentHull(), torpedoDamage);
	MYTRACE("logcombat")(MT::LEVEL_DEBUG, dam);
*/

	int toHull = 0;
	// Wenn das feindliche Schiff keine ablative H�llenpanzerung hat, dann gehen 10% des Schadens sofort
	// auf die H�lle
	if (CS->m_pShip->GetHull()->GetAblative() == FALSE)
		toHull = (int)(torpedoDamage * DAMAGE_TO_HULL);
	// wenn wir schilddurchschlagende Torpedos haben und das feindliche Schiff keine auf unseren Torpedo eingestellten
	// regenerativen Schilde hat, dann machen wir kompletten Schaden an der H�lle. Wenn wir Torpedos haben, die alle
	// Schilde durchdringen, dann machen wir hier immer kompletten Schaden an der H�lle.
	if ((CTorpedoInfo::GetPenetrating(m_byType) == TRUE && CS->GetActRegShields() == FALSE)
		|| CTorpedoInfo::GetIgnoreAllShields(m_byType) == TRUE || CS->m_bCanUseShields == FALSE)
		toHull = torpedoDamage;
	CS->m_pShip->GetHull()->SetCurrentHull(-toHull);
	// den restlichen Torpedoschaden ermitteln, welcher nicht direkt auf die H�lle ging
	torpedoDamage -= toHull;

	// Torpedos verlieren ihre Effizienz, wenn sie auf noch relativ starke Schilde treffen. Umso weniger von den Schilden
	// noch aktiv ist, umso st�rker wirkt der Torpedo. Dies gilt jedoch nicht f�r Microtorpedos.
	if (CTorpedoInfo::GetMicro(m_byType) == FALSE)
	{
		UINT maxShield = CS->m_pShip->GetShield()->GetMaxShield();
		UINT curShield = CS->m_pShip->GetShield()->GetCurrentShield();
		float perc = (float)curShield / (float)maxShield;
		if (perc > 0.75f)
			perc = 0.25f;
		else if (perc > 0.50f)
			perc = 0.50f;
		else if (perc > 0.25f)
			perc = 0.75f;
		else
			perc = 1.0f;
		torpedoDamage = (int)(torpedoDamage * perc);
	}
	// Wenn die Torpedoart doppelten Schaden an den Schilden macht, dann einfach diesen hier *2 nehmen
	if (CTorpedoInfo::GetDoubleShieldDmg(m_byType))
		torpedoDamage *= 2;
	// Wenn die Torpedoart die Schilde kollabieren lassen kann, dann dies hier m�glicherweise tun
	if (CTorpedoInfo::GetCollapseShields(m_byType))
		if (rand()%100 < 5)	// 5% Chance die Schilde zusammenbrechen zu lassen
			CS->m_pShip->GetShield()->SetCurrentShield(0);
	// Der komplette Schaden geht auf die Schilde
	if ((int)CS->m_pShip->GetShield()->GetCurrentShield() - torpedoDamage >= 0)
		CS->m_pShip->GetShield()->SetCurrentShield((int)CS->m_pShip->GetShield()->GetCurrentShield() - torpedoDamage);
	// Der Schaden geht teilweise auf Schilde und H�lle oder nur auf die H�lle
	else
	{
		// Wenn die Torpedoart doppelten Schaden an der H�lle macht, dann einfach diesen hier *2 nehmen
		if (CTorpedoInfo::GetDoubleHullDmg(m_byType))
			torpedoDamage *= 2;
		// Schaden der noch auf die H�lle geht
		CS->m_pShip->GetHull()->SetCurrentHull((int)CS->m_pShip->GetShield()->GetCurrentShield() - torpedoDamage);
		CS->m_pShip->GetShield()->SetCurrentShield(0);
	}

	// Wenn wir schilddurchschlagende Torpedos haben und das feindliche Schiff regenerative Schilde, so kann es diese
	// auf unsere Waffen einstellen
	if (CTorpedoInfo::GetPenetrating(m_byType) == TRUE && CS->m_pShip->GetShield()->GetRegenerative() == TRUE)
		CS->ActRegShield();
	// Wenn der Torpedo die F�higkeit hat die Man�vriebarkeit des Ziels auf 0 zu setzen, so wird dies hier beachtet
	if (CTorpedoInfo::GetReduceManeuver(m_byType))
		if (rand()%100 < 5)	// Nur mit einer 5% Wahrscheinlichkeit tritt dies auch ein
			CS->SetManeuverability(0);

/*CString damage;
	dam.Format("Torpedoschaden an Schiff # %s (%s) # nachher: Schilde: %d H�lle: %d Torpedoschaden: %d",
		CS->m_pShip->GetShipName(), CS->m_pShip->GetShipClass(), CS->m_pShip->GetShield()->GetCurrentShield(),
		CS->m_pShip->GetHull()->GetCurrentHull(), torpedoDamage);
//	//AfxMessageBox(damage);
	MYTRACE("logcombat")(MT::LEVEL_DEBUG, "%s", damage);

	// Merken welche Rasse das Schiff schlussendlich zerst�rt hat
	if (!CS->m_pShip->IsAlive())
		CS->m_pKilledByShip = m_pShipFiredTorpedo;
*/
}

// Diese private Funktion setzt den Torpedo auf den n�chsten Punkt seiner Flugbahn. Diese Funktion nur aufrufen,
// wenn der Torpedo keinen Schaden an irgendeinem Schiff gemacht hat. Hat der Torpedo aus irgendeinem Grund
// seine maximale Lebensdauer erreicht, gibt die Funktion ein TRUE zur�ck, anonsten FALSE.
BOOLEAN CTorpedo::FlyToNextPosition()
{
	// TORPEDOSPEED vorr�cken
	// unsere aktuelle Position im Raum
	vec3i a = m_KO;
	// die Zielposition im Raum
	vec3i b = m_TargetKO;
	// der Abstand zwischen diesen beiden Punkten
	int distance = a.Distance(b);
	if (distance == 0)
		return TRUE;
	short speed = TORPEDOSPEED;
	if (speed > distance)
		speed = (short)distance;

	float multi = (float)speed / (float)distance;
	vec3i temp = b - a;
	// Runden durch floor und +0.5
	temp.x = (int)(floor)(temp.x * multi + 0.5); temp.y = (int)(floor)(temp.y * multi + 0.5); temp.z = (int)(floor)(temp.z * multi + 0.5);
	m_KO = a + temp;
	//m_KO = a + (b-a) * (speed/distance);
	// Wegcounter hochrechnen
	m_iDistance += TORPEDOSPEED;
	if (m_iDistance >= MAX_TORPEDO_RANGE)
		return TRUE;
	return FALSE;
}

// Diese private Funktion gibt ein TRUE zur�ck, wenn der Torpedo sein Ziel getroffen hat, andernfalls wird ein
// FALSE zur�ckgegeben. Ob ein Torpedo trifft h�ngt von mehreren Parametern ab, welche diese Funktion alle
// ber�cksichtigt. Wenn er nicht trifft ruft die Funktion selbstst�ndig <code>FlyToNextPosition<code> auf. Als
// Parameter wird ein Zeiger auf das Schiff �bergeben, welches vielleicht getroffen wird und die Distanz zwischen
// unserem Torepdo und dem Zielschiff.
BOOLEAN CTorpedo::PerhapsImpact(CCombatShip* CS, USHORT minDistance)
{
	// Ein Torpedo trifft nicht 100prozentig. Ob ein Torpedo trifft h�ngt von mehreren Parametern ab.
	// Parameter von denen die Wahrscheinlichkeit eines Treffers abh�ngen sind
	//	+ m_iModi		... Modifikator durch Crew welche den Torpedo abfeuerte, den Acc durch den Werfer
	//						und den ToHitBonus durch die Man�vriebarkeit
	m_iModi += CCombatShip::GetToHitBoni(m_byManeuverability, CS->m_byManeuverability);

	//	+ m_iDistance	... Schon zur�ckgelegte Strecke des Torpedos
	//	+ CS			... Das Zielschiff selbst

	// je gr��er m_iDistance + minDistance ist, desto geringer wird die Trefferwahrscheinlichkeit (Wert zwischen 0 und 200)
	// je schneller (setzt sich aus Schiffstyp und Geschwindigkeit zusammen) das Zielschiff im Kampf ist,
	//		umso geringer ist die Trefferwahrscheinlichkeit
	// je erfahrener die Crew des Zielschiffes ist,	umso geringer ist die Trefferwahrscheinlichkeit

	// zum Algorithmus:

	// ein Torpedo trifft normalerweise mit einer Wahrscheinlichkeit von XX%.
	// m_iMode - (m_iDistance+minDistance)*0.1 - CS->Man�vrierbarkeit*2 - CS->Crewerfahrung???
	short probability = m_iModi - (short)((m_iDistance + minDistance) * 0.1) -
		CCombatShip::GetToHitMali(m_byManeuverability, CS->m_byManeuverability) - CS->GetCrewExperienceModi();

	if (CS->m_pShip->GetShipSize() == SHIP_SIZE::SMALL)
		probability = (short)(probability * 0.66);
	else if (CS->m_pShip->GetShipSize() == SHIP_SIZE::BIG)
		probability = (short)(probability * 1.33);
	else if (CS->m_pShip->GetShipSize() >= SHIP_SIZE::HUGE)
		probability = (short)(probability * 1.66);

	// Die Wahrscheinlichkeit betr�gt mindestens 10% f�r einen Einschlag
	probability = max(probability, 10);

	short random = rand()%100;	// {0,99}

//CString q;
//q.Format("vielleicht Einschlag in Schiff %s Modi: %d Wahrscheinlichkeit: %d Randomwert: %d zur�ckgelegte Strecke: %d MinDistanz: %d ToHitMali: %d, Crew: %d", CS->m_pShip->GetShipClass(), m_iModi, probability, random, m_iDistance, minDistance, CCombatShip::GetToHitMali(m_byManeuverability, CS->m_byManeuverability), CS->GetCrewExperienceModi());
//	MYTRACE("general")(MT::LEVEL_INFO, s);
//CString r;
//r.Format("m_iDistance: %d\nminDistance: %d\nprobability: %d\nrandom: %d",m_iDistance,minDistance,probability,random);
//	AfxMessageBox(s);
//MYTRACE("general")(MT::LEVEL_INFO, "Message:%q %r", q, r);

	if (probability > random)
	{
		MakeDamage(CS);
		return TRUE;
	}
	else
		return FALSE;
}
