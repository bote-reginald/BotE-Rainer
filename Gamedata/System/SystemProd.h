/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "BuildingInfo.h"
#include <map>


class CSystemProd :	public CObject
{
	friend class CSystem;
	friend class CSystemAI;
public:
	DECLARE_SERIAL (CSystemProd)
	// Standardkonstruktor
	CSystemProd(void);

	CSystemProd(const CSystemProd&);

	// Destruktor
	virtual ~CSystemProd(void);

	CSystemProd& operator=(const CSystemProd&);


	struct RESEARCHBONI {
		public:
			RESEARCHBONI() {
				memset(nBoni, 0, sizeof(nBoni));
			}
			RESEARCHBONI(short bio, short energy, short comp, short prop, short cons, short weap) {
				nBoni[0] = bio;
				nBoni[1] = energy;
				nBoni[2] = comp;
				nBoni[3] = prop;
				nBoni[4] = cons;
				nBoni[5] = weap;
			}
			RESEARCHBONI& operator+=(const RESEARCHBONI& o) {
				nBoni[0] += o.nBoni[0];
				nBoni[1] += o.nBoni[1];
				nBoni[2] += o.nBoni[2];
				nBoni[3] += o.nBoni[3];
				nBoni[4] += o.nBoni[4];
				nBoni[5] += o.nBoni[5];
				return *this;
			}
			short nBoni[6];
	};

	// Die Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	// Funktionen geben die einzelnen Produktionen des Systems zur�ck.
	int GetFoodProd() const {return m_iFoodProd;}
	int GetMaxFoodProd() const {return m_iMaxFoodProd;}
	int GetIndustryProd() const {return m_iIndustryProd;}
	int GetEnergyProd() const {return m_iEnergyProd;}
	int GetMaxEnergyProd() const {return m_iMaxEnergyProd;}
	int GetSecurityProd() const {return m_iSecurityProd;}
	int GetResearchProd() const {return m_iResearchProd;}
	int GetTitanProd() const {return m_iTitanProd;}
	int GetDeuteriumProd() const {return m_iDeuteriumProd;}
	int GetDuraniumProd() const {return m_iDuraniumProd;}
	int GetCrystalProd() const {return m_iCrystalProd;}
	int GetIridiumProd() const {return m_iIridiumProd;}
	int GetResourceProd(BYTE res) const;
	short GetDeritiumProd() const {return m_iDeritiumProd;}
	short GetCreditsProd() const {return m_iCreditsProd;}
	short GetMoralProd() const {return m_iMoralProd;}
	static short GetMoralProdEmpireWide(const CString& sRaceID) {return m_iMoralProdEmpireWide[sRaceID];}
	static std::map<CString, short>* GetMoralProdEmpireWide(void) {return &m_iMoralProdEmpireWide;}
	// Forschungstechboni
	short GetBioTechBoni() const {return m_iBioTechBoni;}
	short GetEnergyTechBoni() const {return m_iEnergyTechBoni;}
	short GetCompTechBoni() const {return m_iCompTechBoni;}
	short GetPropulsionTechBoni() const {return m_iPropulsionTechBoni;}
	short GetConstructionTechBoni() const {return m_iConstructionTechBoni;}
	short GetWeaponTechBoni() const {return m_iWeaponTechBoni;}
	// Geheimdienstboni
	short GetInnerSecurityBoni() const {return m_iInnerSecurityBoni;}
	short GetEconomySpyBoni() const {return m_iEconomySpyBoni;}
	short GetEconomySabotageBoni() const {return m_iEconomySabotageBoni;}
	short GetScienceSpyBoni() const {return m_iResearchSpyBoni;}
	short GetScienceSabotageBoni() const {return m_iResearchSabotageBoni;}
	short GetMilitarySpyBoni() const {return m_iMilitarySpyBoni;}
	short GetMilitarySabotageBoni() const {return m_iMilitarySabotageBoni;}
	// sonstige Informationen
	BOOLEAN GetShipYard() const {return m_bShipYard;}
	SHIP_SIZE::Typ GetMaxBuildableShipSize() const {return m_iBuildableShipSizes;}
	short GetShipYardEfficiency() const {return m_iShipYardEfficiency;}
	BOOLEAN GetBarrack() const {return m_bBarrack;}
	short GetBarrackEfficiency() const {return m_iBarracksEfficiency;}
	int GetShieldPower() const {return m_iShieldPower;}
	int GetShipDefend() const {return m_iShipDefend;}
	int GetGroundDefend() const {return m_iGroundDefend;}
	short GetGroundDefendBoni() const {return m_iGroundDefendBoni;}
	int GetScanPower() const {return m_iScanPower;}
	BYTE GetScanRange() const {return m_iScanRange;}
	short GetShipTraining() const {return m_iShipTraining;}
	short GetTroopTraining() const {return m_iTroopTraining;}
	short GetResistance() const {return m_iResistance;}
	BYTE GetAddedTradeRoutes() const {return m_iAddedTradeRouts;}
	short GetIncomeOnTradeRoutes() const {return m_iIncomeOnTradeRoutes;}
	short GetShipRecycling() const {return m_iShipRecycling;}
	short GetBuildingBuildSpeed() const {return m_iBuildingBuildSpeed;}
	short GetUpdateBuildSpeed() const {return m_iUpdateBuildSpeed;}
	short GetShipBuildSpeed() const {return m_iShipBuildSpeed;}
	short GetTroopBuildSpeed() const {return m_iTroopBuildSpeed;}
	BOOLEAN GetResourceDistributor(BYTE res) const { return m_bResourceDistributor[res]; }

	RESEARCHBONI GetResearchBoni() const;

	// zum Schreiben der Membervariabeln
	void AddMoralProd(short moralAdd) {m_iMoralProd += moralAdd;}

	static void SetMoralProdEmpireWide(const CString& sRaceID, short value) {m_iMoralProdEmpireWide[sRaceID] = value;}

// sonstige Funktionen
	// Funktion berechnet die neuen Produktionen im System die sich durch das �bergebene Geb�ude ergeben
	void CalculateProduction(const CBuildingInfo*);

	/// Funktion zum deaktivieren bestimmter Produktionen
	/// @param vDisabledProductions Array mit zu deaktiverenden Produktionen
	void DisableProductions(const bool* vDisabledProductions);

	/// Funktion setzt die Creditproduktion auf 0.
	void DisableCreditsProduction() { m_iCreditsProd = min(m_iCreditsProd, 0); }

	// Funktion berechnet die m�gliche Produktions�nderung durch die Moral im System
	void IncludeSystemMoral(short moral);

	/// Funktion setzt die <code>m_iMoralProdEmpireWide</code> wieder auf NULL.
	static void ResetMoralEmpireWide(void) {m_iMoralProdEmpireWide.clear();}

	// Resetfunktion f�r die Klasse CSystemProd
	void Reset();

private:
	// Produktionswerte der einzelnen Geb�ude
	int m_iFoodProd;				///< Nahrungsproduktion in dem System
	int m_iMaxFoodProd;				///< Nahrungsproduktion in dem System ohne Bev�lkerungsabzug
	int m_iIndustryProd;			///< Industrieproduktion in dem System
	int m_iEnergyProd;				///< Energyproduktion in dem System
	int m_iMaxEnergyProd;			///< Energieproduktion ohne den Energieverbrauch mancher Geb�ude
	int m_iSecurityProd;			///< Geheimdienstproduktion in dem System
	int m_iResearchProd;			///< Forschungspunkteproduktion in dem System
	int m_iTitanProd;				///< Titanproduktion in dem System
	int m_iDeuteriumProd;			///< Deuteriumproduktion in dem System
	int m_iDuraniumProd;			///< Duraniumproduktion in dem System
	int m_iCrystalProd;				///< Crystalproduktion in dem System
	int m_iIridiumProd;				///< Iridiumproduktion in dem System
	short m_iDeritiumProd;			///< Deritiumproduktion in dem System
	short m_iCreditsProd;			///< Creditsproduktion in dem System, abh�ngig von der Bev�lkerung
	short m_iMoralProd;				///< Moralproduktion aller Geb�ude (also die +x Moral)
	static std::map<CString, short> m_iMoralProdEmpireWide;	///< Moralproduktion imperiumsweit f�r alle Imperien
	// Forschungstechboni
	short m_iBioTechBoni;
	short m_iEnergyTechBoni;
	short m_iCompTechBoni;
	short m_iPropulsionTechBoni;
	short m_iConstructionTechBoni;
	short m_iWeaponTechBoni;
	// Sicherheitsboni
	short m_iInnerSecurityBoni;
	short m_iEconomySpyBoni;
	short m_iEconomySabotageBoni;
	short m_iResearchSpyBoni;
	short m_iResearchSabotageBoni;
	short m_iMilitarySpyBoni;
	short m_iMilitarySabotageBoni;
	// sonstige Informationen
	BOOLEAN m_bShipYard;
	SHIP_SIZE::Typ m_iBuildableShipSizes;
	short m_iShipYardEfficiency;
	BOOLEAN m_bBarrack;
	short m_iBarracksEfficiency;
	int m_iShieldPower;
	short m_iShieldPowerBoni;
	int m_iShipDefend;
	short m_iShipDefendBoni;
	int m_iGroundDefend;
	short m_iGroundDefendBoni;
	int m_iScanPower;
	short m_iScanPowerBoni;
	BYTE m_iScanRange;
	short m_iScanRangeBoni;
	short m_iShipTraining;
	short m_iTroopTraining;
	short m_iResistance;				// Bestechungsresistenz
	BYTE m_iAddedTradeRouts;
	short m_iIncomeOnTradeRoutes;
	short m_iShipRecycling;
	short m_iBuildingBuildSpeed;
	short m_iUpdateBuildSpeed;
	short m_iShipBuildSpeed;
	short m_iTroopBuildSpeed;
	BOOLEAN m_bResourceDistributor[DERITIUM + 1];		///< Ressourcenverteiler (Nahrung, Titan, Deuterium, Duranium, Kristalle, Iridium, Deritium)
};
