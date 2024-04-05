use std::net::{TcpListener, TcpStream};
use std::io::{Read, Write};
// use std::thread;

extern "C" {
    pub fn crypt_chacha20_poly1305_encrypt(
        iv: *const u8,
        iv_len: u32,
        aad: *const u8,
        aad_len: u32,
        key: *const u8,
        key_len: u32,
        constant: *const u8,
        constant_len: u32,
        plaintext: *const u8,
        plaintext_len: u32,
        ciphertext: *mut u8,
        ciphertext_len: u32,
        aead_output: *mut u8,
        aead_output_len: u32,
    ) -> crypt_status;

    pub fn crypt_chacha20_poly1305_decrypt(
        iv: *const u8,
        iv_len: u32,
        aad: *const u8,
        aad_len: u32,
        key: *const u8,
        key_len: u32,
        constant: *const u8,
        constant_len: u32,
        aead_input: *mut u8,
        aead_input_len: u32,
        plaintext: *mut u8,
        plaintext_len: u32,
    ) -> crypt_status;

    pub fn crypt_test() -> crypt_status;
}

#[derive(PartialEq)]
pub enum crypt_status {
    CRYPT_OKAY = 0,
    CRYPT_FAILURE = 1, 
    CRYPT_NULL_PTR = 2,
    CRYPT_BAD_BUFFER_LEN = 3, 
    CRYPT_BAD_KEY = 4,
    CRYPT_BAD_NONCE = 5,
    CRYPT_BAD_IV = 6,
    CRYPT_UNSUPPORTED_ALGO = 7,
    CRYPT_COMPUTATION_ERROR = 8,
    CRYPT_INVALID_TEXT = 9,
}

fn main() {
    // bind() binds the IP addr and the port on which we listen for new incoming connections
    // bind() will return a Result<T, E>, which indicates that it is possible for binding to fail
    // and unwrap() stops the program if error happens
    let listener = TcpListener::bind("127.0.0.1:8081").unwrap();

    // incoming method returns an iterator of Result<TcpStream, std::io::Error>. Each TcpStream is 
    // an incoming connection from a client
    let mut status: crypt_status =  unsafe {crypt_test() };
    if status == crypt_status::CRYPT_OKAY{
        println!("Crypt_test return code: is crypt_okay");
    }

    static iv: [u8; 8] = [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08];
    static aad: [u8; 8] = [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08]; 
    static key: [u8; 32] = [
        0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a, 0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0,
        0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09, 0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0
    ]; 
    static constant: [u8; 4] = [0x00, 0x00, 0x00, 0x01]; 
    let mut ciphertext = [0; 1024];
    let mut aead_output = [0; 1040];

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                // thread::spawn(move || handle_client(stream));
                handle_client(stream);

                // unsafe{
                //     let status = crypt_chacha20_poly1305_decrypt(
                //         iv.as_ptr(),
                //         iv.len() as u32,
                //         aad.as_ptr(),
                //         aad.len() as u32,
                //         key.as_ptr(),
                //         key.len() as u32,
                //         constant.as_ptr(),
                //         constant.len() as u32,
                //         buffer.as_ptr(),
                //         user_input.len() as u32,
                //         ciphertext.as_mut_ptr(),
                //         user_input.len() as u32,
                //         aead_output.as_mut_ptr(),
                //         (user_input.len() + 16) as u32,
                //     );
                // }
            }
            Err(e) => {
                eprintln!("Connection failed: {}", e);
            }
        }
    }

    println!("Connection established");
}

fn handle_client(mut stream: TcpStream) {
    let mut buffer = [0; 1024];

    while let Ok(n) = stream.read(&mut buffer) {
        if n == 0 {
            // connection is closed 
            break;
        }
        stream.write_all(&buffer[0..n]).unwrap();
        match std::str::from_utf8(&buffer[0..n]) {
            Ok(s) => println!("Received: {}", s),
            Err(e) => println!("Error decoding UTF-8: {}", e),
        }
    }

}