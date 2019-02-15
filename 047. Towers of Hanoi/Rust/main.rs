fn remove_last_char(string: &mut String, ch: char) {
	let last: char = match string.chars().last() {
		Some(c) => c,
		None => '\0'
	};

	if last != '\0' && last == ch {
		string.pop();
	}
}

fn hanoi(disks: u32) -> usize {
	2_usize.pow(disks) - 1
}

fn main() {
	let mut input: String = String::new();
	println!("Enter the number of disks:");
	std::io::stdin().read_line(&mut input).expect("Couldn't read number of disks");

	remove_last_char(&mut input, '\n');
	remove_last_char(&mut input, '\r');

	let disks: i32 = match input.parse::<i32>() {
		Ok(n) => n,
		Err(_) => -1
	};

	if disks <= 0 {
		println!("Error, disks number less or equals zero");
		return;
	}
	
	println!("Number of iterations: {}", hanoi(disks as u32));
}


