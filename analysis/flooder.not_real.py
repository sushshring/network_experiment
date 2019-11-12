notify_client()
loop 60 seconds // Flooding sequence
    notify_client()
    loop 5 seconds: // Watermarking Sequence
        send_burst_packet_to_client()
    end loop
    notify_client() // Notify end of watermarking sequence
    sleep(5) // Idling Sequence
    notify_client() // Notify end of idling sequence
end loop
notify_client() // Notify Start of control sequence
loop while client.active()
    sleep(5) // Watermarking Sequence
    sleep(5) // Idling Sequence
end loop

