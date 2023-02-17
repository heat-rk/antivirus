object Dependencies {
    val kodein = "org.kodein.di:kodein-di-framework-compose:${Versions.kodein}"
    val decompose = "com.arkivanov.decompose:decompose:${Versions.decompose}"
    val decomposeExtensions = "com.arkivanov.decompose:extensions-compose-jetbrains:${Versions.decompose}"
    val essenty = "com.arkivanov.essenty:parcelable:${Versions.essenty}"
    val sqliteJdbc = "org.xerial:sqlite-jdbc:${Versions.sqliteJdbc}"
    val exposedCore = "org.jetbrains.exposed:exposed-core:${Versions.exposed}"
    val exposedDao = "org.jetbrains.exposed:exposed-dao:${Versions.exposed}"
    val exposedJdbc = "org.jetbrains.exposed:exposed-jdbc:${Versions.exposed}"
    val immutableCollections = "org.jetbrains.kotlinx:kotlinx-collections-immutable:${Versions.immutableCollections}"
    val coroutinesSwing = "org.jetbrains.kotlinx:kotlinx-coroutines-swing:${Versions.coroutines}"

    object Versions {
        val compose = "1.2.0"
        val decompose = "1.0.0"
        val kodein = "7.14.0"
        val sqliteJdbc = "3.39.2.0"
        val exposed = "0.41.1"
        val immutableCollections = "0.3.5"
        val essenty = "0.7.0"
        val coroutines = "1.6.4"
    }
}