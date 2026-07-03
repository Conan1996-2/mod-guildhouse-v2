DELETE FROM creature_template WHERE entry=900000;

INSERT INTO creature_template (entry, name, subname, minlevel, maxlevel, faction, npcflag, unit_class, type, speed_walk, speed_run, AIName, ScriptName)
VALUES (900000, 'Guild House Broker', 'Guild Services', 80, 80, 35, 1, 1, 7, 1, 1.14286, '', 'GuildHouseNpc');
