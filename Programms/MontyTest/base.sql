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
