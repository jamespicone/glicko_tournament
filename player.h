#ifndef PLAYER__H__
#define PLAYER__H__

#include <QString>
#include <QSet>

#include <cmath>

struct Player
{	
	QString Name;
	QSet<int> Played;
	
	int Id;
	
	double Rating;
	double RD;
	
	unsigned int Wincount;
	unsigned int Losscount;
	
	operator QString() const
	{
		QString ret = QString::number(Id) + "|";
		ret += Name + "|";
		ret += QString::number(Rating) + "|";
		ret += QString::number(RD) + "|";
		ret += QString::number(Wincount) + "|";
		ret += QString::number(Losscount) + "|";
		foreach (int i, Played) {ret += QString::number(i) + ",";}
		if (Played.count()) {ret.chop(1);}
		
		return ret;
	}
	
	Player()
	:Name(""), Id(-1), Rating(-1), RD(-1), Wincount(0), Losscount(0)
	{
	}
	
	Player(QString n, int i)
	:Name(n), Id(i), Rating(1500), RD(350), Wincount(0), Losscount(0)
	{}
	
	double EffectiveRating() const
	{
		return Rating - RD;
	}
	
	bool hasPlayed(const Player* const other)
	{
		return Played.contains(other->Id);
	}
	
	void setPlayed(const Player* const other)
	{
		Played.insert(other->Id);
	}
	
	bool operator<(const Player& rhs) const
	{
		if (EffectiveRating() == rhs.EffectiveRating())
		{
			return Name < rhs.Name;
		}
		
		return EffectiveRating() < rhs.EffectiveRating();
	}
	
	bool operator>(const Player& rhs) const
	{
		if (EffectiveRating() == rhs.EffectiveRating())
		{
			return Name > rhs.Name;
		}
		
		return EffectiveRating() > rhs.EffectiveRating();
	}
	
	bool operator==(const Player& rhs) const
	{
		return Name == rhs.Name && EffectiveRating() == rhs.EffectiveRating();
	}
	
	bool operator!=(const Player& rhs) const
	{
		return *this != rhs;
	}
	
	static bool UpdatePlayers(Player* winner, Player* loser)
	{
		if (!winner->hasPlayed(loser))
		{
			double winner_rating = winner->Rating;
			double winner_rd = winner->RD;
			
			winner->update_player(loser->Rating, loser->RD, 1);
			loser->update_player(winner_rating, winner_rd, 0);
			
			winner->setPlayed(loser);
			loser->setPlayed(winner);
			return true;
		}
		else
		{
			return false;
		}
	}
	
	static double g(double RD_i)
	{
	  double q = (std::log(10.0) / 400.0);
	  double ret = 1 + 3 * q * q * RD_i * RD_i / (M_PI * M_PI);
	  return 1 / std::sqrt(ret);
	}

	double E(double r_i, double RD_i) const
	{
	  double ret = 1 + std::pow(10.0, (Rating - r_i) * g(RD_i) / -400);
	  return 1 / ret;
	}

	double d_sqr(double r_i, double RD_i) const
	{
	  double q = (std::log(10.0) / 400.0);
	  double ret = q * q * g(RD_i) * g(RD_i) * E(r_i, RD_i) * (1 - E(r_i, RD_i));
	  return 1 / ret;
	}

	void update_player(double r_i, double RD_i, double s)
	{
	  Wincount += s;
	  Losscount += 1 - s;
	  double div = 1 / (RD*RD) + 1 / d_sqr(r_i, RD_i);
	  double q = (std::log(10.0) / 400.0);
	  
	  Rating = Rating + (q / div) * g(RD_i) * (s - E(r_i, RD_i));;
	  RD = std::sqrt(1 / div);
	}
};

#endif