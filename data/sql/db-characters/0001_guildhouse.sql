DROP TABLE IF EXISTS `guildhouse`;
CREATE TABLE `guildhouse`
(
    `guildId` INT UNSIGNED NOT NULL,
    `ownerGuid` INT UNSIGNED NOT NULL,
    `locationId` INT UNSIGNED NOT NULL DEFAULT 0,
    `purchaseDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (`guildId`),
    KEY `idx_owner` (`ownerGuid`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `guildhouse_asset`;
CREATE TABLE `guildhouse_asset`
(
    `assetId` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `guildId` INT UNSIGNED NOT NULL,
    `layoutId` SMALLINT UNSIGNED NOT NULL DEFAULT 1,
    `catalogId` INT UNSIGNED NOT NULL,
    `status` TINYINT UNSIGNED NOT NULL DEFAULT 0,

    `positionX` FLOAT NOT NULL,
    `positionY` FLOAT NOT NULL,
    `positionZ` FLOAT NOT NULL,
    `orientation` FLOAT NOT NULL,

    `createdBy` INT UNSIGNED NOT NULL,
    `enabled` TINYINT(1) NOT NULL DEFAULT 1,
    `createdDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,

    PRIMARY KEY (`assetId`),
    KEY `idx_guild` (`guildId`),
    KEY `idx_layout` (`layoutId`),
    KEY `idx_catalog` (`catalogId`),
    KEY `idx_status` (`status`)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS guildhouse_phase;
CREATE TABLE guildhouse_phase
(
    guildId INT UNSIGNED NOT NULL,
    phaseMask INT UNSIGNED NOT NULL,
    mapId INT UNSIGNED NOT NULL,

    spawnX FLOAT NOT NULL,
    spawnY FLOAT NOT NULL,
    spawnZ FLOAT NOT NULL,
    spawnO FLOAT NOT NULL,

    activeMembers INT UNSIGNED NOT NULL DEFAULT 0,

    PRIMARY KEY (guildId),
    UNIQUE KEY idx_phaseMask (phaseMask)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS guildhouse_spawn;
CREATE TABLE guildhouse_spawn
(
    spawnId INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    guildId INT UNSIGNED NOT NULL,
    assetId INT UNSIGNED NOT NULL,
    phaseMask INT UNSIGNED NOT NULL,
    spawnGuid INT UNSIGNED NOT NULL,
    spawnType TINYINT NOT NULL,
    mapId INT UNSIGNED NOT NULL,

    x FLOAT,
    y FLOAT,
    z FLOAT,
    o FLOAT
);

DROP TABLE IF EXISTS guildhouse_phase_object;
CREATE TABLE guildhouse_phase_object
(
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    guildId INT UNSIGNED NOT NULL,
    phaseMask INT UNSIGNED NOT NULL,
    assetId INT UNSIGNED NOT NULL,
    spawnGuid INT UNSIGNED NOT NULL,

    spawnType TINYINT UNSIGNED NOT NULL,
    -- 0 creature
    -- 1 gameobject

    mapId INT UNSIGNED NOT NULL,

    positionX FLOAT NOT NULL,
    positionY FLOAT NOT NULL,
    positionZ FLOAT NOT NULL,
    orientation FLOAT NOT NULL,

    PRIMARY KEY(id),

    KEY idx_guild(guildId),
    KEY idx_phase(phaseMask),
    KEY idx_asset(assetId),
    KEY idx_spawnGuid(spawnGuid)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS guildhouse_salesman;
CREATE TABLE guildhouse_salesman
(
    guildId INT UNSIGNED NOT NULL,
    spawnGuid INT UNSIGNED NOT NULL,
    phaseMask INT UNSIGNED NOT NULL,
    mapId INT UNSIGNED NOT NULL,

    positionX FLOAT NOT NULL,
    positionY FLOAT NOT NULL,
    positionZ FLOAT NOT NULL,
    orientation FLOAT NOT NULL,

    PRIMARY KEY(guildId),

    KEY idx_phase(phaseMask),
    KEY idx_spawnGuid(spawnGuid)
) ENGINE=InnoDB
DEFAULT CHARSET=utf8mb4
COLLATE=utf8mb4_unicode_ci;
