package ru.heatrk.antivirus.data.models

sealed interface ApiMessage<Body> {
    data class Ok<Body>(val body: Body): ApiMessage<Body>

    data class Fail<Body>(
        val description: String,
        val errorCode: Int? = null
    ): ApiMessage<Body>
}