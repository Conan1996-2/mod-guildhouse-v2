DROP TABLE IF EXISTS `guildhouse_category`;
CREATE TABLE `guildhouse_category`
(
    `categoryId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `parentId` INT UNSIGNED NOT NULL DEFAULT 0,
    `name` VARCHAR(80) NOT NULL,
    `sortOrder` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `enabled` TINYINT(1) NOT NULL DEFAULT 1,

    PRIMARY KEY (`categoryId`),
    KEY `idx_parent` (`parentId`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_catalog`;
CREATE TABLE `guildhouse_catalog`
(
    `catalogId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `categoryId` INT UNSIGNED NOT NULL,

    `name` VARCHAR(100) NOT NULL,

    `price` BIGINT UNSIGNED NOT NULL DEFAULT 0,

    `spawnFlags` INT UNSIGNED NOT NULL DEFAULT 0,
    `behaviorFlags` INT UNSIGNED NOT NULL DEFAULT 0,

    `enabled` TINYINT(1) NOT NULL DEFAULT 1,

    PRIMARY KEY (`catalogId`),

    KEY `idx_category` (`categoryId`),
    KEY `idx_enabled` (`enabled`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_catalog_asset`;
CREATE TABLE `guildhouse_catalog_asset`
(
    `componentId` INT UNSIGNED NOT NULL AUTO_INCREMENT,

    `catalogId` INT UNSIGNED NOT NULL,

    `spawnFlags` INT UNSIGNED NOT NULL DEFAULT 0,
    `behaviorFlags` INT UNSIGNED NOT NULL DEFAULT 0,

    `entryId` INT UNSIGNED NOT NULL DEFAULT 0,
    `displayId` INT UNSIGNED NOT NULL DEFAULT 0,

    `scale` FLOAT NOT NULL DEFAULT 1.0,

    `scriptType` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `scriptData` VARCHAR(100) DEFAULT NULL,

    `xOffset` FLOAT NOT NULL DEFAULT 0,
    `yOffset` FLOAT NOT NULL DEFAULT 0,
    `zOffset` FLOAT NOT NULL DEFAULT 0,
    `oOffset` FLOAT NOT NULL DEFAULT 0,

    `targetMapId` SMALLINT UNSIGNED DEFAULT NULL,
    `targetX` FLOAT DEFAULT NULL,
    `targetY` FLOAT DEFAULT NULL,
    `targetZ` FLOAT DEFAULT NULL,
    `targetO` FLOAT DEFAULT NULL,

    `childCatalogId` INT UNSIGNED DEFAULT NULL,

    `sortOrder` SMALLINT UNSIGNED NOT NULL DEFAULT 0,

    PRIMARY KEY (`componentId`),

    KEY `idx_catalog` (`catalogId`),
    KEY `idx_child` (`childCatalogId`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

INSERT INTO `guildhouse_category` (`categoryId`, `parentId`, `name`, `sortOrder`, `enabled`) VALUES
    (100, 0, "NPC", 0, 1),
    (200, 0, "Portals", 0, 1),
    (300, 0, "Furnature", 0, 1),
    (400, 0, "Plants", 0, 1),
    (101, 100, "Class Trainers", 0, 1),
    (102, 100, "Profession Trainers", 0, 1),
    (103, 100, "Other Trainers", 0, 1),
    (104, 100, "Vendors", 0, 1),
    (301, 300, "Chairs", 0, 1),
    (302, 300, "Tables", 0, 1),
    (303, 300, "Chests", 0, 1;
    
INSERT INTO `guildhouse_category` (`catalogId`, `categoryId`, `name`, `price`, `spawnFlags`, `behaviorFlags`, `enabled`) VALUES
    (1, 101, "Death Knight Trainer", 500000, 1, 4, 1),
    (2, 101, "Druid Trainer", 500000, 1, 1, 1),
    (3, 101, "Druid Trainer", 500000, 1, 2, 1),
    (4, 101, "Hunter Trainer", 500000, 1, 1, 1),
    (5, 101, "Hunter Trainer", 500000, 1, 2, 1),
    (6, 101, "Mage Trainer", 500000, 1, 1, 1),
    (7, 101, "Mage Trainer", 500000, 1, 2, 1),
    (8, 101, "Paladin Trainer", 500000, 1, 1, 1),
    (9, 101, "Paladin Trainer", 500000, 1, 2, 1),
    (10, 101, "Priest Trainer", 500000, 1, 1, 1),
    (11, 101, "Priest Trainer", 500000, 1, 2, 1),
    (12, 101, "Rogue Trainer", 500000, 1, 1, 1),
    (13, 101, "Rogue Trainer", 500000, 1, 2, 1),
    (14, 101, "Shaman Trainer", 500000, 1, 1, 1),
    (15, 101, "Shaman Trainer", 500000, 1, 2, 1),
    (16, 101, "Warrior Trainer", 500000, 1, 1, 1),
    (17, 101, "Warrior Trainer", 500000, 1, 2, 1),
    (18, 101, "Warlock Trainer", 500000, 1, 1, 1),
    (19, 101, "Warlock Trainer", 500000, 1, 2, 1);

