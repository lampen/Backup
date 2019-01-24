/*
Entfernen Sie folgende Informationen auf keinen Fall: / Do not remove following text:
Source code based on the javascript by Arnold Barmettler, www.astronomie.info / www.CalSky.com
based on algorithms by Peter Duffett-Smith's great and easy book
'Practical Astronomy with your Calculator'.
*/
#pragma once
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>

extern double moonPosAz;
extern double moonPosAlt;


namespace Astronomy
{
	class TimeSpan
	{
		friend class SunMoon;
	private:
		int m_hh;
		int m_mm;
		int m_ss;
		double m_dv;

		TimeSpan(double tdiff);

	public:
		TimeSpan();
		~TimeSpan();

		int GetHour();
		int GetMinute();
		int GetSecond();

		float GetTotalHour();
		float GetTotalMinute();
		float GetTotalSecond();

		double GetFloat();
		operator double();

		friend std::ostream& operator<<(std::ostream& os, const TimeSpan& ts);
	};

	enum SIGN
	{
		SIGN_ARIES,
		SIGN_TAURUS,
		SIGN_GEMINI,
		SIGN_CANCER,
		SIGN_LEO,
		SIGN_VIRGO,
		SIGN_LIBRA,
		SIGN_SCORPIO,
		SIGN_SAGITTARIUS,
		SIGN_CAPRICORNUS,
		SIGN_AQUARIUS,
		SIGN_PISCES
	};

	enum LUNARPHASE
	{
		LP_NEW_MOON,
		LP_WAXING_CRESCENT_MOON,    
		LP_FIRST_QUARTER_MOON,     
		LP_WAXING_GIBBOUS_MOON,     
		LP_FULL_MOON,            
		LP_WANING_GIBBOUS_MOON,   
		LP_LAST_QUARTER_MOON,     
		LP_WANING_CRESCENT_MOON,   
	};

	class SunMoon
	{
	private:
		double m_Lat;
		double m_Lon;
		struct tm m_datumzeit;
		int m_Zone;
		double m_DeltaT;
		double m_JD;
		TimeSpan m_GMST;
		TimeSpan m_LMST;
		double m_SunDistance;
		double m_SunDistanceObserver;
		double m_SunLon;
		TimeSpan m_SunRA;
		double m_SunDec;
		double m_SunAz;
		double m_SunAlt;
		double m_SunDiameter;
		TimeSpan m_SunAstronomicalTwilightMorning;
		TimeSpan m_SunNauticalTwilightMorning;
		TimeSpan m_SunCivilTwilightMorning;
		TimeSpan m_SunRise;
		TimeSpan m_SunTransit;
		TimeSpan m_SunSet;
		TimeSpan m_SunCivilTwilightEvening;
		TimeSpan m_SunNauticalTwilightEvening;
		TimeSpan m_SunAstronomicalTwilightEvening;
		SIGN m_SunSign;
		double m_MoonDistance;
		double m_MoonDistanceObserver;
		double m_MoonLon;
		double m_MoonLat;
		TimeSpan m_MoonRA;
		double m_MoonDec;
		double m_MoonAz;
		double m_MoonAlt;
		double m_MoonDiameter;
		TimeSpan m_MoonRise;
		TimeSpan m_MoonTransit;
		TimeSpan m_MoonSet;
		double m_MoonPhaseNumber;
		double m_MoonAge;
		LUNARPHASE m_MoonPhase;
		SIGN m_MoonSign;

	public:
		SunMoon();
		SunMoon(double latitude, double longitude, struct tm* datetime, double deltaT);
		SunMoon(double latitude, double longitude, struct tm* datetime, double deltaT, int zone);
		~SunMoon();

		void SetInput(double latitude, double longitude, struct tm* datetime, double deltaT);
		void SetInput(double latitude, double longitude, struct tm* datetime, double deltaT, int zone);

		double GetLat();

		double GetLon();

		struct tm GetDateTime();

		double GetZone();

		double GetDeltaT();

		double GetJD();

		TimeSpan GetGMST();

		TimeSpan GetLMST();

		double GetSunDistance();

		double GetSunDistanceObserver();

		double GetSunLon();

		TimeSpan GetSunRA();

		double GetSunDec();

		double GetSunAz();

		double GetSunAlt();

		double GetSunDiameter();

		TimeSpan GetSunAstronomicalTwilightMorning();

		TimeSpan GetSunNauticalTwilightMorning();

		TimeSpan GetSunCivilTwilightMorning();

		TimeSpan GetSunRise();

		TimeSpan GetSunTransit();

		TimeSpan GetSunSet();

		TimeSpan GetSunCivilTwilightEvening();

		TimeSpan GetSunNauticalTwilightEvening();

		TimeSpan GetSunAstronomicalTwilightEvening();

		SIGN GetSunSign();

		double GetMoonDistance();

		double GetMoonDistanceObserver();

		double GetMoonLon();

		double GetMoonLat();

		TimeSpan GetMoonRA();

		double GetMoonDec();

		double GetMoonAz();

		double GetMoonAlt();

		double GetMoonDiameter();

		TimeSpan GetMoonRise();

		TimeSpan GetMoonTransit();

		TimeSpan GetMoonSet();

		double GetMoonPhaseNumber();

		double GetMoonAge();

		LUNARPHASE GetMoonPhase();

		SIGN GetMoonSign();

		friend std::ostream& operator<<(std::ostream& os, const SunMoon& sm);
	};

}
