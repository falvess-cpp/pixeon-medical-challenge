/**
 * @file SafeQueue.hpp
 * @brief Implementação de uma fila thread-safe baseada no padrão Produtor-Consumidor.
 * @author Fabiano Souza
 * @date 24/02/2026
 */
 
#ifndef SAFEQUEUE_HPP
#define SAFEQUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @class SafeQueue
 * @brief Fila genérica sincronizada para comunicação entre Threads.
 * 
 * Esta estrutura permite que a Main Thread (Produtor) poste requisições 
 * sem bloquear a interface, enquanto o Background Worker (Consumidor) 
 * aguarda e processa as tarefas de forma sequencial e segura.
 * 
 * @tparam T Tipo de dado armazenado na fila (ex: WorkerRequest).
 */
template <typename T>
class SafeQueue {
public:

    /**
     * @brief Adiciona um item à fila e notifica uma thread em espera.
     * Utiliza std::lock_guard para garantir a segurança atômica da inserção.
     * @param item Objeto a ser enfileirado (suporta move semantics para eficiência).
     */
    void push(T value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(value));
        m_cond.notify_one();
    }

    /**
     * @brief Remove e retorna o item da frente da fila.
     * Caso a fila esteja vazia, a thread chamadora é bloqueada até que 
     * um novo item seja inserido (evita Busy Waiting).
     * @return T O item processado retirado da fila.
     */
    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this] { return !m_queue.empty() || m_stop; });
        
        if (m_stop && m_queue.empty()) return false;

        value = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    /**
     * @brief Solicita a interrupção de todas as threads aguardando na fila.
     * 
     * Define a flag de parada e acorda todas as threads bloqueadas no método pop().
     */
    void stop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
        m_cond.notify_all();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_stop = false;
};

#endif
