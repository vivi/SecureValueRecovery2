import cbor2
from cryptography import x509
from cryptography.x509.verification import Store, PolicyBuilder
import datetime

from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric.ec import ECDSA

ATTESTATION_DOC_PATH = "nitro_attest.bin"
ROOT_CA_CERT_PATH = "nitro_attestation_pki.pem"


# Decode the CBOR object and map it to a COSE_Sign1 structure
def decode_cose_sign1(cose_sign1):
    # Decode the COSE_Sign1 object
    cose_sign1_obj = cbor2.loads(cose_sign1)

    protected_header = cose_sign1_obj[0]
    unprotected_header = cose_sign1_obj[1]
    payload = cose_sign1_obj[2]
    signature = cose_sign1_obj[3]

    attestation_doc = cbor2.loads(payload)

    return (protected_header, unprotected_header, attestation_doc, signature)


with open(ATTESTATION_DOC_PATH, "rb") as f:
    attestation_doc_cbor = f.read()

_, _, attestation_doc, signature = decode_cose_sign1(attestation_doc_cbor)

# print(attestation_doc)

ca_bundle = attestation_doc["cabundle"]
certificate = attestation_doc["certificate"]


def validate_certificate_chain(certificate, ca_bundle, root_ca_cert_path):
    with open(root_ca_cert_path, "rb") as f:
        root_ca_cert = f.read()
    trust_store = Store(x509.load_pem_x509_certificates(root_ca_cert))
    builder = PolicyBuilder().store(trust_store)
    # builder = builder.time(datetime.datetime.now())
    verifier = builder.build_server_verifier(x509.DNSName("aws.nitro-enclaves"))
    certs = [x509.load_der_x509_certificate(cert) for cert in ca_bundle]
    target_cert = x509.load_der_x509_certificate(certificate)
    breakpoint()
    certs.reverse()
    chain = verifier.verify(target_cert, [])
    return chain


def validate_certificate_chain_manual(certificate, ca_bundle, root_ca_cert_path):
    with open(root_ca_cert_path, "rb") as f:
        root_ca_cert = x509.load_pem_x509_certificate(f.read())
    certs = [x509.load_der_x509_certificate(cert) for cert in ca_bundle]  # ordered from root to leaf
    target_cert = x509.load_der_x509_certificate(certificate)

    #  verify to the root
    trusted_pub = root_ca_cert.public_key()
    for intermediate_cert in certs[1:]:
        trusted_pub.verify(intermediate_cert.signature, intermediate_cert.tbs_certificate_bytes, ECDSA(hashes.SHA384()))
        trusted_pub = intermediate_cert.public_key()

    # verify to the target
    trusted_pub.verify(target_cert.signature, target_cert.tbs_certificate_bytes, ECDSA(hashes.SHA384()))


validate_certificate_chain_manual(certificate, ca_bundle, ROOT_CA_CERT_PATH)
