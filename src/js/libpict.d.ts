declare module "libpict" {
	export class LibPict {
		static readonly OK: number;
		static readonly NO_DEVICE: number;
		static readonly NO_PERM: number;
		static readonly OPEN_FAIL: number;
		static readonly OUT_OF_MEM: number;
		static readonly IO_ERR: number;
		static readonly RUNTIME_ERR: number;
		static readonly UNSUPPORTED: number;
		static readonly CHECK_CODE: number;

		constructor();
		connect(): void;
		openSession(): void;
		getDeviceInfo(): object;
		closeSession(): void;
		disconnect(): void;
		doOperation(opcode: number, args: any[], data?: ArrayBuffer): void;
	}
}
