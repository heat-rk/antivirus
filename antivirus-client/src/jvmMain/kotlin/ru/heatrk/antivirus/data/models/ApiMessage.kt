package ru.heatrk.antivirus.data.models

sealed interface ApiMessage<T: Any> {
    data class Ok<T: Any>(val body: T): ApiMessage<T>
    class Fail<T: Any>: ApiMessage<T>
}