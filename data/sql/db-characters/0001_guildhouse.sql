DROP TABLE IF EXISTS `guildhouse`;

CREATE TABLE `guildhouse`
(
    `guildId` INT UNSIGNED NOT NULL,
    `ownerGuid` INT UNSIGNED NOT NULL,
    `purchaseDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (`guildId`),

    KEY `idx_owner` (`ownerGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


DROP TABLE IF EXISTS `guildhouse_asset`;

CREATE TABLE `guildhouse_asset`
(
    `assetId` INT UNSIGNED NOT NULL AUTO_INCREMENT,

    `guildId` INT UNSIGNED NOT NULL,

    `layoutId` SMALLINT UNSIGNED NOT NULL DEFAULT 1,

    `catalogId` INT UNSIGNED NOT NULL,

    `status` TINYINT UNSIGNED NOT NULL DEFAULT 0,

    `phase` INT UNSIGNED NOT NULL,

    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,

    `createdBy` INT UNSIGNED NOT NULL,

    `createdDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (`assetId`),

    KEY `idx_guild` (`guildId`),
    KEY `idx_layout` (`layoutId`),
    KEY `idx_catalog` (`catalogId`),
    KEY `idx_status` (`status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_instance`;

CREATE TABLE `guildhouse_instance`
(
    `instanceId` INT UNSIGNED NOT NULL AUTO_INCREMENT,

    -- The placed package this instance belongs to
    `assetId` INT UNSIGNED NOT NULL,

    -- The blueprint component that created this instance
    `componentId` INT UNSIGNED NOT NULL,

    -- Creature, GameObject, Portal, etc.
    `spawnFlags` INT UNSIGNED NOT NULL,

    -- Spawned GUID in the world
    `guid` INT UNSIGNED NOT NULL,

    -- Guild phase this instance belongs to
    `phase` INT UNSIGNED NOT NULL,

    -- Spawn location
    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,

    PRIMARY KEY (`instanceId`),

    KEY `idx_asset` (`assetId`),
    KEY `idx_component` (`componentId`),
    KEY `idx_guid` (`guid`),
    KEY `idx_phase` (`phase`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
