DROP DATABASE IF EXISTS mybase; #удаляем, если есть такая база
CREATE DATABASE mybase; #создаем базу
USE mybase; # сообщаем, что далее мы будем использовать эту базу

#создаем таблицу
DROP TABLE IF EXISTS Stars;
CREATE TABLE Stars (
	HD INT UNSIGNED NOT NULL AUTO_INCREMENT, #поле ключа
	#HD INT NOT NULL , #номер HD
	Flags CHAR(16) default NULL,
	Alpha_grad INT NOT NULL,
	Alpha_Min DOUBLE NOT NULL,	
	Delta_grad INT NOT NULL,
	Delta_Min DOUBLE NOT NULL,
	Sp CHAR(16) default NULL, 
	W DOUBLE,
	B DOUBLE,
	V DOUBLE,
	R DOUBLE,
	N INT,
	c CHAR(16),
	PRIMARY KEY (HD)	
);

LOAD DATA LOCAL INFILE "F:\\astromonty\\Programms\\MontyTest\\WBVR.DAT" INTO TABLE Stars;

DROP TABLE IF EXISTS Tycho2main;
CREATE TABLE Tycho2main (
	recno 	INT UNSIGNED NOT NULL AUTO_INCREMENT, #поле ключа
	TYC1  	INT UNSIGNED,
	TYC2  	INT UNSIGNED,
	TYC3  	INT UNSIGNED,
	pmRA  	DOUBLE NOT NULL,
	pmDE	DOUBLE NOT NULL,
	BTmag	DOUBLE NOT NULL,	
	VTmag	DOUBLE NOT NULL,
	RA		DOUBLE NOT NULL,
	DE		DOUBLE NOT NULL,	
	PRIMARY KEY (recno),
	INDEX mag (BTmag,VTmag),
	INDEX pos (RA,DE)
);
LOAD DATA LOCAL INFILE "F:\\astromonty\\Programms\\MontyTest\\Tyho2.tsv" INTO TABLE Tycho2main;
DELETE FROM Tycho2main WHERE RA=0 AND DE=0;

DROP TABLE IF EXISTS Tycho2;
CREATE TABLE Tycho2 (
	recno 	INT UNSIGNED NOT NULL AUTO_INCREMENT, #поле ключа
	TYC1  	INT UNSIGNED,
	TYC2  	INT UNSIGNED,
	TYC3  	INT UNSIGNED,
	pmRA  	DOUBLE NOT NULL,
	pmDE	DOUBLE NOT NULL,
	BTmag	DOUBLE NOT NULL,	
	VTmag	DOUBLE NOT NULL,
	RA		DOUBLE NOT NULL,
	DE		DOUBLE NOT NULL,	
	PRIMARY KEY (recno),
	INDEX mag (BTmag,VTmag),
	INDEX pos (RA,DE)
);
INSERT INTO `Tycho2` SELECT * FROM `Tycho2main` WHERE Tycho2main.BTmag<8;

DROP TABLE IF EXISTS WBVRasTycho2;
CREATE TABLE WBVRasTycho2 (
	recno 	INT UNSIGNED NOT NULL AUTO_INCREMENT, #поле ключа
	TYC1  	INT UNSIGNED,
	TYC2  	INT UNSIGNED,
	TYC3  	INT UNSIGNED,
	pmRA  	DOUBLE NOT NULL,
	pmDE	DOUBLE NOT NULL,
	BTmag	DOUBLE NOT NULL,	
	VTmag	DOUBLE NOT NULL,
	RA		DOUBLE NOT NULL,
	DE		DOUBLE NOT NULL,	
	PRIMARY KEY (recno),
	INDEX mag (BTmag,VTmag),
	INDEX pos (RA,DE)
);
INSERT INTO `WBVRasTycho2` SELECT 0,0,0,0,0,0,W,V,(Alpha_Grad+Alpha_min/60)*15,if(Delta_Grad>=0,Delta_Grad+Delta_min/60,Delta_grad-Delta_min/60) FROM `Stars`;

