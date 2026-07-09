DELETE FROM creature_template WHERE entry=900000;

INSERT INTO creature_template (entry, name, subname, minlevel, maxlevel, faction, npcflag, unit_class, type, speed_walk, speed_run, AIName, ScriptName)
VALUES (900000, 'Guild House Broker', 'Guild Services', 80, 80, 35, 1, 1, 7, 1, 1.14286, '', 'GuildHouseBroker');

DELETE FROM creature_template_model WHERE CreatureID=900000;

INSERT INTO creature_template_model (CreatureID, Idx, CreatureDisplayID, DisplayScale, Probability)
VALUES (900000, 0, 25901, 1.0, 1.0);

DELETE FROM creature_template WHERE entry=900001;

INSERT INTO creature_template (entry, name, subname, minlevel, maxlevel, faction, npcflag, unit_class, type, speed_walk, speed_run, AIName, ScriptName)
VALUES (900001, 'Guild House Broker', 'Guild Services', 80, 80, 35, 1, 1, 7, 1, 1.14286, '', 'GuildHouseBroker');

DELETE FROM creature_template_model WHERE CreatureID=900001;

INSERT INTO creature_template_model (CreatureID, Idx, CreatureDisplayID, DisplayScale, Probability)
VALUES (900001, 0, 25901, 1.0, 1.0);

DELETE FROM creature_template WHERE entry=900002;

INSERT INTO creature_template (entry, name, subname, minlevel, maxlevel, faction, npcflag, unit_class, type, speed_walk, speed_run, AIName, ScriptName)
VALUES (900002, 'Guild House Broker', 'Guild Services', 80, 80, 35, 1, 1, 7, 1, 1.14286, '', 'GuildHouseBroker');

DELETE FROM creature_template_model WHERE CreatureID=900002;

INSERT INTO creature_template_model (CreatureID, Idx, CreatureDisplayID, DisplayScale, Probability)
VALUES (900002, 0, 25901, 1.0, 1.0);

DELETE FROM creature_template WHERE entry=900003;

INSERT INTO creature_template (entry, name, subname, minlevel, maxlevel, faction, npcflag, unit_class, type, speed_walk, speed_run, AIName, ScriptName)
VALUES (900003, 'Guild House Broker', 'Guild Services', 80, 80, 35, 1, 1, 7, 1, 1.14286, '', 'GuildHouseBroker');

DELETE FROM creature_template_model WHERE CreatureID=900003;

INSERT INTO creature_template_model (CreatureID, Idx, CreatureDisplayID, DisplayScale, Probability)
VALUES (900003, 0, 25901, 1.0, 1.0);
